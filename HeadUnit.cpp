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

MenuScreen *menu;
AudioPlayerScreen *audioPlayer;
AudioBrowserScreen *audioBrowser;
MediaPlayer *mediaPlayer;
QSettings *settings;
QSqlDatabase *db;

#define CREATE_MUSIC_TABLE "create table music (id integer primary key, artist text, album text, title text, genre text, mrl text)"
#define INSERT_MUSIC_ITEM "insert into music (artist, album, title, genre, mrl) values"


void initializeDB(QSqlDatabase *db, QString path) 
{
  QFileInfo info(path);
  if (!info.isDir()) {
    QString mrl("file://" + info.absFilePath());
    char buf[1024];
    sprintf(buf, "%s ('%s', '%s', '%s', '%s', '%s')", INSERT_MUSIC_ITEM,
	    "Unknown",
	    "Unknown",
	    info.fileName().latin1(),
	    "Unknown",
	    mrl.latin1());
    qWarning(mrl);
    db->exec( buf );	   
    return;
  }

  QDir dir(path, "*.mp3 *.wma *.avi *.mpg", QDir::DirsFirst);
  dir.setMatchAllDirs(true);
  if (!dir.isReadable())
    return;
  
  QFileInfoList *entries = (QFileInfoList *)dir.entryInfoList();
  QFileInfo *fi;
  QString absPath;
  for ( fi = entries->first(); fi; fi = entries->next() ) {
    if (fi->fileName().at(0) != '.')
      initializeDB( db, fi->absFilePath() );
  }  
}

int main( int argc, char **argv )
{
  QApplication a( argc, argv );
  a.addLibraryPath("plugins");

  settings = new QSettings();
  settings->setPath( "mp3car", "headunit" );

  QString dbName("headunit.db");
  if (dbName.isNull()) {
    qWarning("no database defined");
    return 1;
  }
  db = QSqlDatabase::addDatabase( DB_DRIVER );
  if (!db) {
    qWarning("Could not open SQLite driver");
    return 1;
  }
  db->setDatabaseName( dbName );
  db->open();

  menu = new MenuScreen();
  audioPlayer = new AudioPlayerScreen();
  audioBrowser = new AudioBrowserScreen();
  mediaPlayer = new MediaPlayer();

  if (settings->readEntry("headunit/mediapath") == QString::null) {
	  QString mediaPath = 
		  QFileDialog::getExistingDirectory(QString::null,
											0,
											"get media path",
											"Choose location of media files:",
											TRUE);
	  settings->writeEntry("headunit/mediapath", mediaPath);
  }

  if (db->tables().empty()) {
    qWarning("Initializing database: %s", dbName.latin1());
    db->exec( CREATE_MUSIC_TABLE );
    initializeDB(db, settings->readEntry("headunit/mediapath", "./"));
  }

  menu->init();
  audioPlayer->init();
  audioBrowser->init();
  mediaPlayer->init();

  if (menu->isNull() || audioPlayer->isNull() || 
      audioBrowser->isNull() || mediaPlayer->isNull())
    return 1;
  
  menu->show();

  return a.exec();    
}
