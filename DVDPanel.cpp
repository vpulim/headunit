#include <qdatetime.h>
#include <qlabel.h>
#include "HeadUnit.h"
#include "MenuScreen.h"
#include "MediaPlayer.h"
#include "DVDPanel.h"
#include "Button.h"
#include "ApplicationState.h"

DVDPanel::DVDPanel(QWidget *parent) 
  : Panel(parent)
{
  connect( buttons[PLAY], SIGNAL(clicked()), this, SLOT(playpause()) );
  connect( buttons[STOP], SIGNAL(clicked()), this, SLOT(stop()) );
  connect( buttons[EXIT], SIGNAL(clicked()), parent, SLOT(lower()) );
}

void DVDPanel::playpause()
{
  if (mediaPlayer->isPlaying())
    mediaPlayer->dvdPause();
  else if (mediaPlayer->isPaused())
    mediaPlayer->dvdResume();
  else
    mediaPlayer->play();
}

void DVDPanel::stop()
{
  mediaPlayer->stop();
  appState->dvdPos = -1;  
}

bool DVDPanel::next() 
{
  return mediaPlayer->dvdNextChapter();
}

bool DVDPanel::previous() 
{
  return mediaPlayer->dvdPrevChapter();
}
 
void DVDPanel::updateInfo()
{
  if (!mediaPlayer->isDVD())
    return;

  int title, ch;
  long pos, len;
  mediaPlayer->dvdGetLocation(&title, &ch, &pos, &len);
  if (mediaPlayer->isPlaying()) {
    appState->dvdPos = pos;
    appState->dvdTitle = title;
    appState->dvdChapter = ch;
  }
  QTime zero;
  if (title == 0)
    labels[TRACKNAME]->setText(QString("DVD: Menu"));
  else
    labels[TRACKNAME]->setText(QString("DVD: Title %1, Chapter %2")
			       .arg(title)
			       .arg(ch));
  labels[CURRENTTRACKTIME]->setText(zero.addMSecs(pos).toString("hh:mm"));
  labels[TRACKTIME]->setText(zero.addMSecs(len).toString("hh:mm"));
  labels[TIME]->setText(QTime::currentTime().toString("hh:mm:ss"));
  labels[DATE]->setText(QDate::currentDate().toString("d/M/yyyy"));
  if (mediaPlayer->isPlaying())
    labels[STATUS]->setText("PLAY");
  else if (mediaPlayer->isPaused())
    labels[STATUS]->setText("PAUSE");
  else if (mediaPlayer->isStopped())
    labels[STATUS]->setText("STOP");
  labels[VOLUME]->setText(QString::number(mediaPlayer->getVolume()) + "%");
  //  if (mediaPlayer->isPlaying() && len > 0 && pos > len - 2*UPDATE_DELAY)
  //    next();
}
