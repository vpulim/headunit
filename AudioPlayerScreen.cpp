#include <qapplication.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qsqldatabase.h>
#include "HeadUnit.h"
#include "MenuScreen.h"
#include "AudioPlayerScreen.h"
#include "AudioBrowserScreen.h"
#include "XineVideo.h"
#include "SelectionList.h"
#include "MediaItem.h"
#include "Skin.h"

const char *AudioPlayerScreen::buttonKeys[AudioPlayerScreen::NUM_BUTTONS] = 
  {"B03","B09","B04","B17","B02","B01","B05","B06","B08","B07","B10","B11",
   "B12","B13","B14","B15","B16"};

const char *AudioPlayerScreen::labelKeys[AudioPlayerScreen::NUM_LABELS] = 
  {"L04","L03","L08","L07","L08","L02","L01","L09","L10"};

const char *AudioPlayerScreen::slKey = "S01";

#define QUERY_MUSIC_ITEM "select artist, album, title, genre from music "
#define CREATE_PLAYLIST "create table playlist (id integer primary key, mrl text)"
#define INSERT_PLAYLIST "insert into playlist (mrl) values"
#define QUERY_PLAYLIST "select mrl from playlist"
#define DROP_PLAYLIST "drop table playlist"

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

  playList = skin.getSelectionList(slKey, *this);

  updateTimer = new QTimer(this);

  valid = true;
}

void AudioPlayerScreen::init() {
  connect(buttons[LIST], SIGNAL(clicked()), audioBrowser, SLOT(show()) );
  connect(buttons[LIST], SIGNAL(clicked()), this, SLOT(hide()) );
  connect(buttons[EXIT], SIGNAL(clicked()), menu, SLOT(show()) );
  connect(buttons[EXIT], SIGNAL(clicked()), this, SLOT(hide()) );
  connect(buttons[DOWN], SIGNAL(clicked()), playList, SLOT(scrollDown()));
  connect(buttons[UP], SIGNAL(clicked()), playList, SLOT(scrollUp()));
  connect(buttons[PGDOWN], SIGNAL(clicked()), playList,SLOT(scrollPageDown()));
  connect(buttons[PGUP], SIGNAL(clicked()), playList, SLOT(scrollPageUp()));
  connect(buttons[PLAY], SIGNAL(clicked()), this, SLOT(play()) );
  connect(buttons[STOP], SIGNAL(clicked()), this, SLOT(stop()) );
  connect(buttons[PREV], SIGNAL(clicked()), this, SLOT(previous()) );
  connect(buttons[NEXT], SIGNAL(clicked()), this, SLOT(next()) );
  connect(buttons[VOLUP], SIGNAL(clicked()), xineVideo, SLOT(volumeUp()) );
  connect(buttons[VOLDN], SIGNAL(clicked()), xineVideo, SLOT(volumeDown()) );
  connect(buttons[MUTE], SIGNAL(clicked()), xineVideo, SLOT(volumeMute()) );
  connect(buttons[VISU], SIGNAL(clicked()), xineVideo, SLOT(showAsVis()) );
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(savePlayList()) );
  connect(audioBrowser, SIGNAL(fileSelected(const QString&)), 
	  this, SLOT(addFileToPlayList(const QString&)));
  connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateInfo()));
  updateTimer->changeInterval(500);
  loadPlayList();
}

void AudioPlayerScreen::play()
{
  int index = playList->currentItem();
  if (playList->count() == 0)
    return;
  if (index == -1) {
    playList->setCurrentItem(0);
    index = 0;
  }
  MediaItem *mi = (MediaItem *)(playList->item(index));
  xineVideo->open(mi->mrl());
  xineVideo->play();
}

void AudioPlayerScreen::stop()
{  
  if (xineVideo->isPlaying())
    xineVideo->stop();
}

void AudioPlayerScreen::previous()
{
  playList->scrollUp();
  if (xineVideo->isPlaying()) {
    stop();
    play();
  }
}

void AudioPlayerScreen::next()
{
  playList->scrollDown();
  if (xineVideo->isPlaying()) {
    stop();
    play();
  }
}

void AudioPlayerScreen::loadPlayList()
{
  QSqlQuery q( QString(QUERY_PLAYLIST) );
  if (q.isActive()) {
    while (q.next()) {
      addMRLToPlayList(q.value(0).toString());
    }      
  }
}

void AudioPlayerScreen::savePlayList()
{
  db->exec( DROP_PLAYLIST );
  db->exec( CREATE_PLAYLIST );
  MediaItem *mi;
  char buf[1024];
  for (uint i=0; i<playList->count(); i++) {
    mi = (MediaItem *)(playList->item(i));
    sprintf(buf, "%s ('%s')", INSERT_PLAYLIST, mi->mrl().latin1());
    db->exec( buf );
  }
}

void AudioPlayerScreen::addMRLToPlayList(const QString &mrl)
{
  QSqlQuery q( QString(QUERY_MUSIC_ITEM) + "where mrl='" + mrl + "'" );
  if (q.isActive()) {
    if (q.next()) {
      MediaItem *mi = new MediaItem(mrl, 
				    q.value(0).toString(),
				    q.value(1).toString(),
				    q.value(2).toString(),
				    q.value(3).toString());
      playList->insertItem(mi);
    }
  }
}

void AudioPlayerScreen::addFileToPlayList(const QString &file)
{
  addMRLToPlayList(QString(file).prepend("file://"));
}

void AudioPlayerScreen::updateInfo()
{
  int pos = 0, len = 0;
  if (xineVideo->isOpened()) {
    xineVideo->getPosition(&pos,&len);
  }
  QTime zero;
  const char *title = xineVideo->getTitle();
  labels[TRACKNAME]->setText(title?title:"");
  labels[CURRENTTRACKTIME]->setText(zero.addMSecs(pos).toString("mm:ss"));
  labels[TRACKTIME]->setText(zero.addMSecs(len).toString("mm:ss"));
  labels[TIME]->setText(QTime::currentTime().toString("hh:mm:ss"));
  labels[DATE]->setText(QDate::currentDate().toString("d/M/yyyy"));
  labels[STATUS]->setText(xineVideo->isPlaying()?"PLAY":"STOP");
  labels[VOLUME]->setNum(xineVideo->getVolume());
}
