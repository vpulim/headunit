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

const char *AudioPlayerScreen::indicatorKeys[AudioPlayerScreen::NUM_INDICATORS] = 
  {"SHUFFLE", "REPEAT", "MODE"};

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

  for (int i=0; i<NUM_INDICATORS; i++) {
    indicators[i] = skin.getIndicator(indicatorKeys[i], *this);
  }

  listView = skin.getSelectionList(slKey, *this);
  updateTimer = new QTimer(this);
  valid = true;
  nextHeldLong = prevHeldLong = false;
  diff = 0;
}

void AudioPlayerScreen::init() {
  connect(buttons[LIST], SIGNAL(clicked()), audioBrowser, SLOT(display()) );
  connect(buttons[EXIT], SIGNAL(clicked()), this, SLOT(lower()) );
  connect(buttons[DOWN], SIGNAL(clicked()), listView, SLOT(scrollDown()));
  connect(buttons[UP], SIGNAL(clicked()), listView, SLOT(scrollUp()));
  connect(buttons[PGDOWN], SIGNAL(clicked()), listView,SLOT(scrollPageDown()));
  connect(buttons[PGUP], SIGNAL(clicked()), listView, SLOT(scrollPageUp()));
  connect(buttons[PLAY], SIGNAL(clicked()), this, SLOT(playpause()) );
  connect(buttons[STOP], SIGNAL(clicked()), this, SLOT(stop()) );
  connect(buttons[PREV], SIGNAL(pressed()), this, SLOT(prevPressed()) );
  connect(buttons[PREV], SIGNAL(released()), this, SLOT(prevReleased()) );
  connect(buttons[NEXT], SIGNAL(pressed()), this, SLOT(nextPressed()) );
  connect(buttons[NEXT], SIGNAL(released()), this, SLOT(nextReleased()) );
  connect(buttons[MODE], SIGNAL(released()), this, SLOT(changeMode()) );
  connect(listView, SIGNAL(selected(int)), this, SLOT(play()) );
  connect(buttons[VOLUP], SIGNAL(clicked()), mediaPlayer, SLOT(volumeUp()) );
  connect(buttons[VOLDN], SIGNAL(clicked()), mediaPlayer, SLOT(volumeDown()) );
  connect(buttons[MUTE], SIGNAL(clicked()), mediaPlayer, SLOT(volumeMute()) );
  connect(buttons[VISU], SIGNAL(clicked()), mediaPlayer, SLOT(showAsVis()) );
  connect(audioBrowser, SIGNAL(folderSelected(QString&, bool, int)), 
	  this, SLOT(loadFolder(QString&, bool, int)));
  connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateInfo()));
  updateTimer->changeInterval(UPDATE_DELAY);

  loadFolder(appState->folderPath, appState->folderPlus, appState->folderIndex);
  playMode = (appState->playMode + NUM_MODES - 1) % NUM_MODES;
  changeMode();  // to draw the right indicator
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

bool AudioPlayerScreen::previous()
{
  if (playMode == MODE_SHUFFLE || playMode == MODE_REPEATSHUFFLE) {
    if (shufflePos == 0 && playMode != MODE_REPEATSHUFFLE)
      return false;
    shufflePos = (shufflePos + shuffleList.size() - 1) % shuffleList.size();
    listView->setCurrentItem(shuffleList[shufflePos]);
  }
  else {
    listView->setCurrentItem(appState->folderIndex);
    if (!listView->scrollUp()) {
      if (playMode == MODE_REPEAT)
        listView->setCurrentItem(listView->count()-1);
      else
        return false; 
    }
  }
  if (!mediaPlayer->isStopped())
    play();
  return true;
}

bool AudioPlayerScreen::next()
{
  if (playMode == MODE_SHUFFLE || playMode == MODE_REPEATSHUFFLE) {
    shufflePos = (shufflePos + 1) % shuffleList.size();
    if (shufflePos == 0 && playMode != MODE_REPEATSHUFFLE)
      return false;
    listView->setCurrentItem(shuffleList[shufflePos]);
  }
  else {
    listView->setCurrentItem(appState->folderIndex);
    if (!listView->scrollDown()) {
      if (playMode == MODE_REPEAT)
        listView->setCurrentItem(0);
      else
        return false; 
    }
  }
  if (!mediaPlayer->isStopped())
    play();
  return true;
}

void AudioPlayerScreen::changeMode() 
{
  playMode = (playMode + 1) % NUM_MODES;
  switch (playMode) {
    case MODE_NORMAL:
      indicators[IREPEAT]->hide();
      indicators[ISHUFFLE]->hide();
      break;
    case MODE_REPEAT:
      indicators[IREPEAT]->show();
      break;
    case MODE_SHUFFLE:
      indicators[IREPEAT]->hide();
      indicators[ISHUFFLE]->show();
      break;
    case MODE_REPEATSHUFFLE:
      indicators[IREPEAT]->show();
      break;
  }
}

void AudioPlayerScreen::nextPressed()
{
  nextPressTime.start();
}

void AudioPlayerScreen::nextReleased()
{
  nextHeldLong = false;
  if (nextPressTime.elapsed() < SLOW_SEEK_DELAY)
    next();
}

void AudioPlayerScreen::prevPressed()
{
  prevPressTime.start();
}

void AudioPlayerScreen::prevReleased()
{
  prevHeldLong = false;
  if (prevPressTime.elapsed() < SLOW_SEEK_DELAY)
    previous();
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

  // setup random shuffle list
  int *temp = (int *)malloc(sizeof(int)*playList.size());
  srand(QTime::currentTime().msec());
  for (int i=0; i<playList.size(); i++) {
    if (i==0) {
      temp[0]=0;
      continue;
    }
    int randPos = rand() % i;
    temp[i] = temp[randPos];
    temp[randPos] = i;
  }
  shuffleList.clear();
  shuffleList.reserve(playList.size());
  for (int i=0;i<playList.size();i++)
    shuffleList.append(temp[i]);
  shufflePos = 0;
  delete(temp);

  // setup selection list
  listView->clear();
  int size = playList.size();
  for (int i=0; i<size; i++) {
    listView->insertItem(QString::number(i+1)+". "+playList[i].artist() + " - " + playList[i].title());
  }

  listView->setCurrentItem(index);
  appState->folderPath = p;
  appState->folderPlus = plus;
  appState->folderIndex = index;
  play();
}

void AudioPlayerScreen::updateInfo()
{
  long pos = 0, len = 0;
  if (mediaPlayer->isOpened()) {
    mediaPlayer->getPosition(&pos,&len);
  }
  QTime zero;
  QString artist = mediaPlayer->getOpened().artist();
  QString title = mediaPlayer->getOpened().title();
  labels[TRACKNAME]->setText(artist + " - " + title);
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
  if (mediaPlayer->isPlaying() && len > 0 && pos > len - 2*UPDATE_DELAY)
    endOfStreamReached();

  // check if prev or next buttons are being held down
  long offset = 0;
  if (buttons[NEXT]->isDown()) {
    if (nextHeldLong || nextPressTime.elapsed() > FAST_SEEK_DELAY) {
        offset = FAST_SEEK_MSECS;
        nextHeldLong = true;  // must use this to prevent int overflow
    }
    else if (nextPressTime.elapsed() > MED_SEEK_DELAY)
      offset = MED_SEEK_MSECS;
    else if (nextPressTime.elapsed() > SLOW_SEEK_DELAY)
      offset = SLOW_SEEK_MSECS;
    if (offset) mediaPlayer->setPosition(pos + offset);
  }
  else if (buttons[PREV]->isDown()) {
    if (prevHeldLong || prevPressTime.elapsed() > FAST_SEEK_DELAY) {
        offset = FAST_SEEK_MSECS;
        prevHeldLong = true;  // must use this to prevent int overflow
    }
    else if (prevPressTime.elapsed() > MED_SEEK_DELAY)
      offset = MED_SEEK_MSECS;
    else if (prevPressTime.elapsed() > SLOW_SEEK_DELAY)
      offset = SLOW_SEEK_MSECS;
    if (diff && offset) {
      // this is a hack for xine, sine it takes a while for pos and len to
      // be updated correctly
      if (len > 0) {
	mediaPlayer->setPosition(len - offset);
	diff = 0;
      }
      return;
    }
    if (offset) {
      if (pos - offset < 0) {
        diff = offset;
        if (previous()) {
          mediaPlayer->getPosition(&pos,&len);
	  if (len > 0) {
	    mediaPlayer->setPosition(len - diff);
	    diff = 0;
	  }
        }
        else {
          mediaPlayer->setPosition(0);
        }
      }
      else
        mediaPlayer->setPosition(pos - offset);
    }
  }
}
