#include <qapplication.h>
#include <qtimer.h>
#include <qdatetime.h>
#include "DBHandler.h"
#include "HeadUnit.h"
#include "MenuScreen.h"
#include "AudioPlayerScreen.h"
#include "AudioBrowserScreen.h"
#include "MediaPlayer.h"
#include "SelectionList.h"
#include "ApplicationState.h"
#include "MediaItem.h"
#include "Skin.h"

const char *AudioPlayerScreen::buttonKeys[AudioPlayerScreen::NUM_BUTTONS] = 
  {"VISU","SHUFFLE/REPEAT","MIXER","MASTERMUTE","VOL-","VOL+","PREV","PLAY","NEXT","STOP","EJECT","LIST",
   "EXIT","PGDOWN","DOWN","UP","PGUP"};

const char *AudioPlayerScreen::labelKeys[AudioPlayerScreen::NUM_LABELS] = 
  {"TRACKNUMBER","TRACKNAME","PLAYLIST","VOLUME","STATUS","CURRENTTRACKTIME","TRACKTIME","TIME","DATE"};

const char *AudioPlayerScreen::slKey = "S01";

AudioPlayerScreen::AudioPlayerScreen() : FunctionScreen("AudioPlayer")
{
  Skin skin("Audio_Player.skin");
  if (skin.isNull()) {
    return;
  }
  skin.set(*this);
  for (int i=0; i<NUM_BUTTONS; i++) {
    buttons[i] = skin.getButton(buttonKeys[i], *this);
  }
  buttons[UP]->setAutoRepeat(true);
  buttons[DOWN]->setAutoRepeat(true);
  buttons[PGUP]->setAutoRepeat(true);
  buttons[PGDOWN]->setAutoRepeat(true);
  buttons[VOLUP]->setAutoRepeat(true);
  buttons[VOLDN]->setAutoRepeat(true);
  
  for (int i=0; i<NUM_LABELS; i++) {
    labels[i] = skin.getLabel(labelKeys[i], *this);
  }

  listView = skin.getSelectionList(slKey, *this);

  updateTimer = new QTimer(this);

  valid = true;
}

void AudioPlayerScreen::init() {
  connect(buttons[LIST], SIGNAL(clicked()), audioBrowser, SLOT(show()) );
  connect(buttons[LIST], SIGNAL(clicked()), this, SLOT(hide()) );
  connect(buttons[EXIT], SIGNAL(clicked()), menu, SLOT(show()) );
  connect(buttons[EXIT], SIGNAL(clicked()), this, SLOT(hide()) );
  connect(buttons[DOWN], SIGNAL(clicked()), listView, SLOT(scrollDown()));
  connect(buttons[UP], SIGNAL(clicked()), listView, SLOT(scrollUp()));
  connect(buttons[PGDOWN], SIGNAL(clicked()), listView,SLOT(scrollPageDown()));
  connect(buttons[PGUP], SIGNAL(clicked()), listView, SLOT(scrollPageUp()));
  connect(buttons[PLAY], SIGNAL(clicked()), this, SLOT(playpause()) );
  connect(buttons[STOP], SIGNAL(clicked()), this, SLOT(stop()) );
  connect(buttons[PREV], SIGNAL(clicked()), this, SLOT(previous()) );
  connect(buttons[NEXT], SIGNAL(clicked()), this, SLOT(next()) );
  connect(listView, SIGNAL(selected(int)), this, SLOT(play()) );
  connect(buttons[VOLUP], SIGNAL(clicked()), mediaPlayer, SLOT(volumeUp()) );
  connect(buttons[VOLDN], SIGNAL(clicked()), mediaPlayer, SLOT(volumeDown()) );
  connect(buttons[MUTE], SIGNAL(clicked()), mediaPlayer, SLOT(volumeMute()) );
  connect(buttons[VISU], SIGNAL(clicked()), mediaPlayer, SLOT(showAsVis()) );
  connect(audioBrowser, SIGNAL(folderSelected(QString&, bool, int)), 
	  this, SLOT(loadFolder(QString&, bool, int)));
  connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateInfo()));
  updateTimer->changeInterval(500);

  loadFolder(appState->folderPath, appState->folderPlus, appState->folderIndex);
  play();
}

void AudioPlayerScreen::play()
{
  int index = listView->currentItem();
  if (listView->count() == 0)
    return;
  if (index == -1) {
    listView->setCurrentItem(0);
    index = 0;
  }
  mediaPlayer->open(playList[index]);
  mediaPlayer->play();
  appState->folderIndex = index;
}

void AudioPlayerScreen::playpause()
{
  if (appState->folderIndex == listView->currentItem()) {
    if (mediaPlayer->isPlaying())
      mediaPlayer->pause();
    else if (mediaPlayer->isPaused())
      mediaPlayer->play();
    else
      play();
  }
  else
    play();
}

void AudioPlayerScreen::stop()
{  
  mediaPlayer->stop();
}

void AudioPlayerScreen::previous()
{
  listView->setCurrentItem(appState->folderIndex);
  listView->scrollUp();
  if (!mediaPlayer->isStopped())
    play();
}

void AudioPlayerScreen::next()
{
  listView->setCurrentItem(appState->folderIndex);

  if (!listView->scrollDown())
    return;
  
  if (!mediaPlayer->isStopped())
    play();
}

void AudioPlayerScreen::endOfStreamReached()
{
  next();
}

void AudioPlayerScreen::loadFolder(QString& path, bool plus, int index)
{
  QString p = path;
  if (path.isNull()) {
    // if the path is null, load first folder in database
    p = dbHandler->getKeyForId(1);
    plus = false;
  }
  dbHandler->loadMusicList(p, plus, playList);

  listView->clear();
  int size = playList.size();
  for (int i=0; i<size; i++) {
    listView->insertItem(playList[i].title());
  }

  listView->setCurrentItem(index);
  appState->folderPath = p;
  appState->folderPlus = plus;
  appState->folderIndex = index;
}

void AudioPlayerScreen::updateInfo()
{
  int pos = 0, len = 0;
  if (mediaPlayer->isOpened()) {
    mediaPlayer->getPosition(&pos,&len);
  }
  QTime zero;
  QString title = mediaPlayer->getOpened().title();
  labels[TRACKNAME]->setText(title.isNull()?QString("Unknown"):title);
  labels[CURRENTTRACKTIME]->setText(zero.addMSecs(pos).toString("mm:ss"));
  labels[TRACKTIME]->setText(zero.addMSecs(len).toString("mm:ss"));
  labels[TIME]->setText(QTime::currentTime().toString("hh:mm:ss"));
  labels[DATE]->setText(QDate::currentDate().toString("d/M/yyyy"));
  if (mediaPlayer->isPlaying())
    labels[STATUS]->setText("PLAY");
  else if (mediaPlayer->isPaused())
    labels[STATUS]->setText("PAUSE");
  else if (mediaPlayer->isStopped())
    labels[STATUS]->setText("STOP");
  labels[VOLUME]->setText(QString::number(mediaPlayer->getVolume()) + "%");
  if (mediaPlayer->isPlaying() && pos == len)
    endOfStreamReached();
}
