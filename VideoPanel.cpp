#include "HeadUnit.h"
#include "AudioPlayerScreen.h"
#include "VideoBrowserScreen.h"
#include "MenuScreen.h"
#include "VideoPanel.h"
#include "Button.h"
#include "MediaPlayer.h"
#include "ApplicationState.h"
#include "DBHandler.h"

VideoPanel::VideoPanel(QWidget *parent) 
  : Panel(parent)
{
  connect( buttons[PLAY], SIGNAL(clicked()), this, SLOT(playpause()) );
  connect( buttons[STOP], SIGNAL(clicked()), this, SLOT(stop()) );
  connect( buttons[EXIT], SIGNAL(clicked()), parent, SLOT(lower()) );
  connect( buttons[LIST], SIGNAL(clicked()), videoBrowser, SLOT(display()) );  
  connect( videoBrowser, SIGNAL(folderSelected(QString&, bool, int, long)), 
	  this, SLOT(loadFolder(QString&, bool, int, long)));
}

void VideoPanel::init()
{
  loadFolder(appState->videoPath, FALSE, appState->videoIndex, 
	     appState->videoPos);
}

void VideoPanel::display() 
{
  bool wasVideo = appState->function == ApplicationState::VIDEO;
  appState->function = ApplicationState::VIDEO;
  if (!mediaPlayer->isVideo()) {
    if (mediaPlayer->isOpened())
      mediaPlayer->closeItem();
    loadFolder(appState->videoPath, FALSE, appState->videoIndex,
	       appState->videoPos);
  }
  if (!mediaPlayer->isPlaying())
    play();
  if (wasVideo)
    Panel::display();
}

void VideoPanel::loadFolder(QString& path, bool plus, int index, long pos)
{
  QString p = path;
  if (path.isNull()) {
    // if the path is null, load first video folder in database
    p = dbHandler->firstVideoFolder();
    index = 0;
    pos = -1;
  }
  dbHandler->loadMediaList(p, FALSE, playList);

  appState->videoPath = p;
  appState->videoIndex = index;
  if (appState->function == ApplicationState::VIDEO && pos != -1) {
    play();
    mediaPlayer->setPosition(pos);
  }
}

void VideoPanel::play()
{
  if (playList.size() == 0)
    return;
  if (appState->videoIndex < 0 || appState->videoIndex >= playList.size()) {
    appState->videoIndex = 0;
  }
  mediaPlayer->openItem(playList[appState->videoIndex]);
  mediaPlayer->play();
}

void VideoPanel::playpause()
{
  if (mediaPlayer->isPlaying())
    mediaPlayer->pause();
  else if (mediaPlayer->isPaused())
    mediaPlayer->play();
  else
    play();
}

void VideoPanel::stop()
{  
  mediaPlayer->stop();
  appState->videoPos = -1;
}

bool VideoPanel::previous()
{
  appState->videoIndex--;
  if (appState->videoIndex < 0) 
    appState->videoIndex = playList.size() - 1;
  if (!mediaPlayer->isStopped())
    play();
  return true;
}

bool VideoPanel::next()
{
  appState->videoIndex++;
  if (appState->videoIndex == playList.size()) 
    appState->videoIndex = 0;
  if (!mediaPlayer->isStopped())
    play();
  return true;
}

