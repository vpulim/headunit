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

MenuScreen *menu = NULL;
AudioPlayerScreen *audioPlayer = NULL;
AudioBrowserScreen *audioBrowser = NULL;
MediaPlayer *mediaPlayer = NULL;
ConfigDialog *configDialog;
DBHandler *dbHandler;
ApplicationState *appState;
QSettings settings;

#define ERROR 1
#define QDESTROY(w) if (w != NULL) {w->close(TRUE);w=NULL;}

int initializeGui()
{
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
  
  menu->raise();
  menu->show();

  return 0;
}

void destroyGui()
{
  QDESTROY(mediaPlayer);
  QDESTROY(audioPlayer);
  QDESTROY(audioBrowser);
  QDESTROY(menu);
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
