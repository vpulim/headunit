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

MenuScreen *menu;
AudioPlayerScreen *audioPlayer;
AudioBrowserScreen *audioBrowser;
MediaPlayer *mediaPlayer;
ConfigDialog *configDialog;
DBHandler *dbHandler;
QSettings settings;

#define ERROR 1

QString getMediaPath(void)
{
    return settings.readEntry(QString("headunit/mediapath"));
}

QString askMediaPath(void)
{
    QString mediaPath = 
      QFileDialog::getExistingDirectory(getMediaPath(),0,
					"get media path",
					"Choose location of media files:",
					TRUE);
    qWarning("writing setting: %s", mediaPath.latin1());
    return mediaPath;
}

void setMediaPath(QString path)
{
    if (path == QString::null)
	return;
    settings.writeEntry(QString("headunit/mediapath"), path);
}

int initializeGui(void)
{
  configDialog = new ConfigDialog();
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

  dbHandler=new DBHandler();
  
  Q_CHECK_PTR(dbHandler);
  
  if (getMediaPath() == QString::null) {
    setMediaPath(askMediaPath());
  }
  
  if (initializeGui()==ERROR)
      return ERROR;
  
  return a.exec();    
}
