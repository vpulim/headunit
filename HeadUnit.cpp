#include <qapplication.h>
#include <qsettings.h>
#include <qsqldatabase.h>
#include <qdir.h>
#include <qfiledialog.h>
#include "HeadUnit.h"
#include "MenuScreen.h"
#include "AudioPlayerScreen.h"
#include "AudioBrowserScreen.h"
#include "MediaPlayer.h"
#include "DBHandler.h"
#include "ConfigDialog.h"
#include "ApplicationState.h"

MenuScreen *menu;
AudioPlayerScreen *audioPlayer;
AudioBrowserScreen *audioBrowser;
MediaPlayer *mediaPlayer;
ConfigDialog *configDialog;
DBHandler *dbHandler;
ApplicationState *appState;
QSettings settings;

#define ERROR 1

int initializeGui(void)
{
  configDialog = new ConfigDialog();
  appState = new ApplicationState();
  dbHandler=new DBHandler(); 
  if (dbHandler->isEmpty())
	configDialog->exec();
  dbHandler->loadAppState();
  QObject::connect(qApp, SIGNAL(aboutToQuit()), dbHandler, SLOT(saveAppState()) );

  menu = new MenuScreen();
  audioPlayer = new AudioPlayerScreen();
  audioBrowser = new AudioBrowserScreen();
  mediaPlayer = new MediaPlayer();

  menu->init();
  audioPlayer->init();
  audioBrowser->init();
  mediaPlayer->init();
  
  if (menu->isNull() || 
      audioPlayer->isNull() || 
      audioBrowser->isNull() || 
      mediaPlayer->isNull())
    return ERROR;
  
  menu->show();
  
  return 0;
}

int main( int argc, char **argv )
{
  QApplication a( argc, argv );
  a.addLibraryPath("plugins");

  settings.setPath( "mp3car", "headunit" );

  if (initializeGui()==ERROR)
      return ERROR;
  
  return a.exec();    
}
