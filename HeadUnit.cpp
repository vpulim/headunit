#include <qapplication.h>
#include <qsettings.h>
#include <qsqldatabase.h>
#include <qdir.h>
#include <qfiledialog.h>
#include "HeadUnit.h"
#include "MenuScreen.h"
#include "AudioPlayerScreen.h"
#include "AudioBrowserScreen.h"
#include "VideoBrowserScreen.h"
#include "MediaPlayer.h"
#include "DBHandler.h"
#include "ConfigDialog.h"
#include "ApplicationState.h"

FunctionScreen *top = NULL;
MenuScreen *menu = NULL;
AudioPlayerScreen *audioPlayer = NULL;
AudioBrowserScreen *audioBrowser = NULL;
VideoBrowserScreen *videoBrowser = NULL;
MediaPlayer *mediaPlayer = NULL;
ConfigDialog *configDialog;
DBHandler *dbHandler;
ApplicationState *appState;
QSettings settings;

#define ERROR 1
#define QDESTROY(w) if (w != NULL) {w->close(TRUE);w=NULL;}

int initializeGui()
{
  top = new FunctionScreen("HeadUnit");
  menu = new MenuScreen(top);
  audioPlayer = new AudioPlayerScreen(top);
  audioBrowser = new AudioBrowserScreen(top);
  videoBrowser = new VideoBrowserScreen(top);
  mediaPlayer = new MediaPlayer(top);

  menu->init();
  audioPlayer->init();
  audioBrowser->init();
  videoBrowser->init();
  mediaPlayer->init();
  
  if (menu->isNull() || 
      audioPlayer->isNull() || 
      audioBrowser->isNull() || 
      videoBrowser->isNull() ||
      mediaPlayer->isNull())
    return ERROR;
  
  top->display();
  menu->display();

  switch (appState->function) {
  case ApplicationState::MUSIC:
    audioPlayer->display();
    break;
  case ApplicationState::VIDEO:
    mediaPlayer->showAsVideo();
    break;
  default:
    break;
  }

  return 0;
}

void destroyGui()
{
  QDESTROY(mediaPlayer);
  QDESTROY(audioPlayer);
  QDESTROY(audioBrowser);
  QDESTROY(videoBrowser);
  QDESTROY(menu);
  QDESTROY(top);
}

int main( int argc, char **argv )
{
  QApplication a( argc, argv );
  a.addLibraryPath("plugins");

  settings.setPath( "mp3car", "headunit" );

  configDialog = new ConfigDialog();
  appState = new ApplicationState();
  dbHandler=new DBHandler(); 
  if (dbHandler->isEmpty())
	configDialog->exec();
  dbHandler->loadAppState();
  QObject::connect(qApp, SIGNAL(aboutToQuit()), dbHandler, SLOT(saveAppState()) );

  if (initializeGui()==ERROR)
      return ERROR;  
  
  return a.exec();    
}
