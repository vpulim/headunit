#include <qapplication.h>
#include <qtimer.h>
#include <qdatetime.h>
#include "HeadUnit.h"
#include "AudioPlayerScreen.h"
#include "MenuScreen.h"
#include "Panel.h"
#include "MediaPlayer.h"
#include "Skin.h"

const char *Panel::keys[Panel::NUM_BUTTONS] = 
  { "MUTE", "VOL-", "VOL+", "ZOOM", "LIST", "MIXER", "PREV", "STOP", "PLAY", "NEXT",
    "UP", "DOWN", "EXIT" };
  enum {TRACKNUMBER, TRACKNAME, VOLUME, STATUS, 
	CURRENTTRACKTIME, TRACKTIME, TIME, DATE, NUM_LABELS};

const char *Panel::labelKeys[Panel::NUM_LABELS] = 
  {"TRACKNUMBER","TRACKNAME", "VOLUME", "STATUS","CURRENTTRACKTIME","TRACKTIME","TIME","DATE"};


Panel::Panel(QWidget *parent) 
  : FunctionScreen("Panel", parent)
{
  loadSkin();
  move(0,parent->size().height() - size().height());
  hide();

  updateTimer = new QTimer(this);
  updateTimer->changeInterval(500);

  connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateInfo()));

  valid = true;
}

void Panel::loadSkin() 
{
  Skin skin("video_player.skin");
  if (skin.isNull()) {
    return;
  }
  skin.set(*this);
  for (int i=0; i<NUM_BUTTONS; i++)
    buttons[i] = skin.getButton(keys[i], *this);
  for (int i=0; i<NUM_LABELS; i++)
    labels[i] = skin.getLabel(labelKeys[i], *this);
}

void Panel::updateInfo()
{
  long pos = 0, len = 0;
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
}
