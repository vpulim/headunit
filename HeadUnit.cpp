#include <qapplication.h>
#include <qsettings.h>
#include <qsqldatabase.h>
#include <qdir.h>
#include "HeadUnit.h"
#include "MenuScreen.h"
#include "AudioPlayerScreen.h"
#include "AudioBrowserScreen.h"
#include "XineVideo.h"

MenuScreen *menu;
AudioPlayerScreen *audioPlayer;
AudioBrowserScreen *audioBrowser;
XineVideo *xineVideo;
QSettings *settings;
QSqlDatabase *db;

#define CREATE_MUSIC_TABLE "create table music (id integer primary key, artist text, album text, title text, genre text, mrl text)"
#define INSERT_MUSIC_ITEM "insert into music (artist, album, title, genre, mrl) values"


void initializeDB(QSqlDatabase *db, QString path) 
{
  QFileInfo info(path);
  if (!info.isDir()) {
    QString mrl("file://" + info.absFilePath());
    xineVideo->open( mrl );
    char buf[1024];
    const char *artist = xineVideo->getArtist();
    const char *album = xineVideo->getAlbum();
    const char *title = xineVideo->getTitle();
    const char *genre = xineVideo->getGenre();
    sprintf(buf, "%s ('%s', '%s', '%s', '%s', '%s')", INSERT_MUSIC_ITEM,
	    artist ? artist : "Unknown",
	    album ? album : "Unknown",
	    title ? title : info.fileName().latin1(),
	    genre ? genre : "Unknown",
	    mrl.latin1());
    qWarning(buf);
    db->exec( buf );	   
    return;
  }

  QDir dir(path, "*.mp3 *.avi *.mpg", QDir::DirsFirst);
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

  settings = new QSettings();
  settings->setPath( "mp3car", "headunit" );

  QString dbName = settings->readEntry( "headunit/db" );
  if (dbName.isNull()) {
    qWarning("no database defined");
    return 1;
  }
  db = QSqlDatabase::addDatabase( "QSQLITE" );
  if (!db) {
    qWarning("Could not open SQLite driver");
    return 1;
  }
  db->setDatabaseName( dbName );
  db->open();

  menu = new MenuScreen();
  audioPlayer = new AudioPlayerScreen();
  audioBrowser = new AudioBrowserScreen();
  xineVideo = new XineVideo();

  if (db->tables().empty()) {
    qWarning("Initializing database: %s", dbName.latin1());
    db->exec( CREATE_MUSIC_TABLE );
    initializeDB(db, settings->readEntry("headunit/mediapath", "./"));
  }

  menu->init();
  audioPlayer->init();
  audioBrowser->init();
  xineVideo->init();

  if (menu->isNull() || audioPlayer->isNull() || 
      audioBrowser->isNull() || xineVideo->isNull())
    return 1;
  
  menu->show();
  
  return a.exec();    
}
