#include <qapplication.h>
#include <qtimer.h>
#include <qdatetime.h>
#include "HeadUnit.h"
#include "AudioPlayerScreen.h"
#include "MenuScreen.h"
#include "XinePanel.h"
#include "XineVideo.h"
#include "Skin.h"

const char *XinePanel::keys[XinePanel::NUM_BUTTONS] = 
  { "B13", "B02", "B01", "B03", "B11", "B04", "B05", "B07", "B06", "B08",
    "B09", "B10", "B12" };

const char *XinePanel::labelKeys[XinePanel::NUM_LABELS] = 
  {"L04","L03","L06","L02","L01","L05","L07","L08"};


XinePanel::XinePanel(QWidget *parent) 
  : FunctionScreen("XinePanel", parent)
{
  loadSkin();
  move(0,parent->size().height() - size().height());
  hide();

  updateTimer = new QTimer(this);
  updateTimer->changeInterval(500);

  connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateInfo()));

  valid = true;
}

void XinePanel::loadSkin() 
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

void XinePanel::updateInfo()
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
