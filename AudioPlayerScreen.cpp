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
#include "MediaItem.h"
#include "Skin.h"

const char *AudioPlayerScreen::buttonKeys[AudioPlayerScreen::NUM_BUTTONS] = 
  {"VISU","SHUFFLE/REPEAT","MIXER","MASTERMUTE","VOL-","VOL+","PREV","PLAY","NEXT","STOP","EJECT","LIST",
   "EXIT","PGDOWN","DOWN","UP","PGUP"};

const char *AudioPlayerScreen::labelKeys[AudioPlayerScreen::NUM_LABELS] = 
  {"TRACKNUMBER","TRACKNAME","PLAYLIST","VOLUME","STATUS","CURRENTTRACKTIME","TRACKTIME","TIME","DATE"};

const char *AudioPlayerScreen::slKey = "S01";

#define QUERY_MUSIC_ITEM "select artist, album, title, genre from music "
#define CREATE_PLAYLIST "create table playlist (id integer primary key, mrl text)"
#define INSERT_PLAYLIST "insert into playlist (mrl) values (:mrl)"
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
  connect(buttons[VOLUP], SIGNAL(clicked()), mediaPlayer, SLOT(volumeUp()) );
  connect(buttons[VOLDN], SIGNAL(clicked()), mediaPlayer, SLOT(volumeDown()) );
  connect(buttons[MUTE], SIGNAL(clicked()), mediaPlayer, SLOT(volumeMute()) );
  connect(buttons[VISU], SIGNAL(clicked()), mediaPlayer, SLOT(showAsVis()) );
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
  mediaPlayer->open(*mi);
  mediaPlayer->play();
}

void AudioPlayerScreen::stop()
{  
  if (mediaPlayer->isPlaying())
    mediaPlayer->stop();
}

void AudioPlayerScreen::previous()
{
  playList->scrollUp();
  if (mediaPlayer->isPlaying()) {
    stop();
    play();
  }
}

void AudioPlayerScreen::next()
{
  if (!playList->scrollDown())
    return;
  
  if (mediaPlayer->isPlaying()) {
    stop();
    play();
  }
}

void AudioPlayerScreen::endOfStreamReached()
{
  next();
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
  QSqlQuery query;
  
  query.prepare( DROP_PLAYLIST );
  query.exec();
  query.prepare( CREATE_PLAYLIST );
  query.exec();
    
  MediaItem *mi;
  for (uint i=0; i<playList->count(); i++) {
    mi = (MediaItem *)(playList->item(i));
    query.prepare( INSERT_PLAYLIST );
    query.bindValue(":mrl",mi->mrl().latin1());
    query.exec();
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
  labels[STATUS]->setText(mediaPlayer->isPlaying()?"PLAY":"STOP");
  labels[VOLUME]->setText(QString::number(mediaPlayer->getVolume()) + "%");
  if (mediaPlayer->isPlaying() && pos == len)
    endOfStreamReached();
}
