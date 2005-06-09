#include <qapplication.h>
#include <qtimer.h>
#include <qdatetime.h>
#include "HeadUnit.h"
#include "AudioPlayerScreen.h"
#include "MenuScreen.h"
#include "Panel.h"
#include "MediaPlayer.h"
#include "Skin.h"
#include "ApplicationState.h"

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
  updateTimer->changeInterval(UPDATE_DELAY);

  connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateInfo()));
  connect(buttons[PREV], SIGNAL(pressed()), this, SLOT(prevPressed()) );
  connect(buttons[PREV], SIGNAL(released()), this, SLOT(prevReleased()) );
  connect(buttons[NEXT], SIGNAL(pressed()), this, SLOT(nextPressed()) );
  connect(buttons[NEXT], SIGNAL(released()), this, SLOT(nextReleased()) );
  connect(buttons[VOLUP], SIGNAL(clicked()), parent, SLOT(volumeUp()) );
  connect(buttons[VOLDN], SIGNAL(clicked()), parent, SLOT(volumeDown()) );
  connect(buttons[MUTE], SIGNAL(clicked()), parent, SLOT(volumeMute()) );

  diff = 0;
  valid = true;
  trackName = " ";
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
  if (mediaPlayer->isDVD())
    return;
  long pos = 0, len = 0;
  if (mediaPlayer->isPlaying() && mediaPlayer->getPosition(&pos,&len)) {
    if (mediaPlayer->isVideo())
      appState->videoPos = pos;
    else if (mediaPlayer->isAudio()) 
      appState->musicPos = pos;
  }
  QTime zero;
  
  /** if no Track Name then get the current track name */
  if (trackName.compare(" ") == 0) {
      trackName = mediaPlayer->getOpened().displayText();
      curTrackName = trackName;
  }
  /** if current Track changes, change the track name */
  if (curTrackName.compare( mediaPlayer->getOpened().displayText() ) != 0) {
      trackName = mediaPlayer->getOpened().displayText();
      curTrackName = trackName;
  }

  labels[TRACKNAME]->setText(trackName);

  /** If the title is larger than the label, scrol text */
  if ( labels[TRACKNAME]->sizeHint().width() > labels[TRACKNAME]->size().width()) {
     /** Add Stars to indicate the end of the title */	
     if (trackName.find("**") == -1)
        trackName.append(" *** ");

     trackName = trackName.right( trackName.length()-1).append(trackName.left(1));
  }

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
    next();

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

void Panel::nextPressed()
{
  nextPressTime.start();
}

void Panel::nextReleased()
{
  nextHeldLong = false;
  if (nextPressTime.elapsed() < SLOW_SEEK_DELAY)
    next();
}

void Panel::prevPressed()
{
  prevPressTime.start();
}

void Panel::prevReleased()
{
  prevHeldLong = false;
  if (prevPressTime.elapsed() < SLOW_SEEK_DELAY)
    previous();
}
