#include <qapplication.h>
#include <qsettings.h>
#include <qsqldatabase.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qcolor.h>
#include "HeadUnit.h"
#include "MenuScreen.h"
#include "AudioPlayerScreen.h"
#include "AudioBrowserScreen.h"
#include "SkinBrowserScreen.h"
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
SkinBrowserScreen *skinBrowser = NULL;
ConfigDialog *configDialog;
DBHandler *dbHandler;
ApplicationState *appState;
QSettings settings;

#define ERROR 1
#define QDESTROY(w) if (w != NULL) {w->close(TRUE);w=NULL;}

int initializeGui()
{
//  settings.setPath( "mp3car", "headunit" );
  if( top == NULL ) {
    top = new FunctionScreen("HeadUnit");
    top->initSkin("wait.skin");
    //top->repaint();
//     top->setBackgroundColor(Qt::black);
//     top->setPaletteForegroundColor(Qt::white);
//     top->setFont( QFont( "Helvetica", 12, QFont::Bold ))
//     top->drawText(10, 10, QString("Loading..."));

    top->display();
  } else {
    top->initSkin("wait.skin");
    top->repaint();
  }
  menu = new MenuScreen(top);
  audioPlayer = new AudioPlayerScreen(top);
  audioBrowser = new AudioBrowserScreen(top);
  videoBrowser = new VideoBrowserScreen(top);
  mediaPlayer = new MediaPlayer(top);
  skinBrowser = new SkinBrowserScreen(top);

  menu->init();
  //menu->display();
  audioPlayer->init();
  audioBrowser->init();
  videoBrowser->init();
  mediaPlayer->init();
  skinBrowser->init();
  
  if (menu->isNull() || 
      audioPlayer->isNull() || 
      audioBrowser->isNull() || 
      videoBrowser->isNull() ||
      mediaPlayer->isNull() ||
      skinBrowser->isNull())
    return ERROR;
  
//   top->display();
  menu->display();
//   mediaPlayer->setEQ();
  switch (appState->function) {
  case ApplicationState::MUSIC:
    audioPlayer->display();
    break;
  case ApplicationState::VIDEO:
    mediaPlayer->showAsVideo();
    break;
  case ApplicationState::DVD:
    mediaPlayer->showAsDVD();
    break;
  default:
    break;
  }

  return 0;
}

void destroyGui()
{
  QDESTROY(skinBrowser);
  QDESTROY(mediaPlayer);
  QDESTROY(audioPlayer);
  QDESTROY(audioBrowser);
  QDESTROY(videoBrowser);
  QDESTROY(menu);
  //QDESTROY(top);
}

void refreshGui() {
  top->initSkin("wait.skin");

   menu->hide();
   videoBrowser->hide();
   audioBrowser->hide();
   mediaPlayer->hide();
   QDESTROY(mediaPlayer);
   audioPlayer->hide();
   skinBrowser->hide();


  //menu->initSkin();
  videoBrowser->initSkin();
  audioBrowser->initSkin();
  //mediaPlayer->initSkin();
  audioPlayer->initSkin();
  mediaPlayer = new MediaPlayer(top);
  skinBrowser->initSkin();
  menu->initSkin();
  
   menu->init();
   audioPlayer->init();
   audioBrowser->init();
   videoBrowser->init();
   mediaPlayer->init();
   skinBrowser->init();

   menu->display();
  
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
