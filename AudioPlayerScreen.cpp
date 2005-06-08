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

AudioPlayerScreen::AudioPlayerScreen(QWidget* parent) 
  : FunctionScreen("AudioPlayer", parent)
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
  connect(audioBrowser, SIGNAL(folderSelected(QString&, bool, int, long)), 
	  this, SLOT(loadFolder(QString&, bool, int, long)));
  connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateInfo()));
  updateTimer->changeInterval(UPDATE_DELAY);

  loadFolder(appState->musicPath, appState->musicPlus, appState->musicIndex,
	     appState->musicPos);
  playMode = (appState->playMode + NUM_MODES - 1) % NUM_MODES;
  changeMode();  // to draw the right indicator
}
/**
 * display() - display screen
 **/
void AudioPlayerScreen::display()
{
  appState->function = ApplicationState::MUSIC;
  if (mediaPlayer->isPlaying()) {
    if (!mediaPlayer->isAudio()) {
      mediaPlayer->closeItem();
      loadFolder(appState->musicPath,appState->musicPlus,appState->musicIndex,
		 appState->musicPos);
    }
  }
  else {
    play();
  }
  FunctionScreen::display();
}

/**
 * play() - play the song.
 **/
void AudioPlayerScreen::play()
{
  int index = listView->currentItem();
  if (listView->count() == 0)
    return;
  if (index == -1) {
    listView->setCurrentItem(0);
    index = 0;
  }
  appState->musicIndex = index;
  mediaPlayer->openItem(playList[index]);
  mediaPlayer->play();
}

/**
 * playpause() - Action of clicking Play button
 **/
void AudioPlayerScreen::playpause()
{
  if (appState->musicIndex == listView->currentItem()) {
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

/**
 * stop() - Action of clicking Stop button
 **/
void AudioPlayerScreen::stop()
{  
  mediaPlayer->stop();
  appState->musicPos = -1;
}

/**
 * previous() - Action of clicking Previous button
 **/
bool AudioPlayerScreen::previous()
{
  if (playMode == MODE_SHUFFLE || playMode == MODE_REPEATSHUFFLE) {
    if (shufflePos == 0 && playMode != MODE_REPEATSHUFFLE)
      return false;
    shufflePos = (shufflePos + shuffleList.size() - 1) % shuffleList.size();
    listView->setCurrentItem(shuffleList[shufflePos]);
  }
  else {
    listView->setCurrentItem(appState->musicIndex);
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

/**
 * next() - Action of clicking Next button
 **/
bool AudioPlayerScreen::next()
{
  if (playMode == MODE_SHUFFLE || playMode == MODE_REPEATSHUFFLE) {
    shufflePos = (shufflePos + 1) % shuffleList.size();
    if (shufflePos == 0 && playMode != MODE_REPEATSHUFFLE)
      return false;
    listView->setCurrentItem(shuffleList[shufflePos]);
  }
  else {
    listView->setCurrentItem(appState->musicIndex);
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

/**
 * playpause() - Action of clicking Play button
 **/
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

/**
 * nextPressed() - Action of pressing Next button (ff)
 **/
void AudioPlayerScreen::nextPressed()
{
  nextPressTime.start();
}

/**
 * nextReleased() - Action of Releasing Next button (stop ff)
 **/
void AudioPlayerScreen::nextReleased()
{
  nextHeldLong = false;
  if (nextPressTime.elapsed() < SLOW_SEEK_DELAY)
    next();
}

/**
 * prevPressed() - Action of pressing Previous button (rew)
 **/
void AudioPlayerScreen::prevPressed()
{
  prevPressTime.start();
}

/**
 * prevReleased() - Action of pressing Previous button (stop rew)
 **/
void AudioPlayerScreen::prevReleased()
{
  prevHeldLong = false;
  if (prevPressTime.elapsed() < SLOW_SEEK_DELAY)
    previous();
}

/**
 * endOfStreamReached() - Song is over, play next.
 **/
void AudioPlayerScreen::endOfStreamReached()
{
  next();
}

/**
 * loadFolder() - Put songs into list.
 **/
void AudioPlayerScreen::loadFolder(QString& path, bool plus, int index, long pos)
{
  QString p = path;
  if (path.isNull()) {
    // if the path is null, load first music folder in database
    p = dbHandler->firstMusicFolder();
    plus = false;
    pos = -1;
    index = 0;
  }
  dbHandler->loadMediaList(p, plus, playList);

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
  if(!plus) 		// If you are not adding to list, clear it
    listView->clear();

  int size = playList.size();
  for (int i=0; i<size; i++) {
    /** This change to the count fixes the problem with numbering when added to the list */
    listView->insertItem(QString::number(listView->count()+1)+". "+playList[i].displayText()); 
  }

  listView->setCurrentItem(index);
  appState->musicPath = p;
  appState->musicPlus = plus;
  appState->musicIndex = index;
  if (appState->function == ApplicationState::MUSIC && pos != -1) {
    play();
    mediaPlayer->setPosition(pos);
  }
}

/**
 * updatateInfo() - Action of timer.
 * 
 **/
void AudioPlayerScreen::updateInfo()
{
  if (!mediaPlayer->isAudio())
    return;  
  long pos = 0, len = 0;
  if (mediaPlayer->isPlaying() && mediaPlayer->getPosition(&pos,&len)) {
    appState->musicPos = pos;
  }
  QTime zero;
  //labels[TRACKNAME]->setText(mediaPlayer->getOpened().displayText());

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
