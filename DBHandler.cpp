#include <qapplication.h>
#include <qsettings.h>
#include <qsqldatabase.h>
#include <qdir.h>
#include "HeadUnit.h"
#include "ConfigDialog.h"
#include "DBHandler.h"
#include "ApplicationState.h"
#include "MediaList.h"
#include "Tag.h"

void DBHandler::loadAppState() 
{
  appState->folderPath = loadStateValue( "folderPath" );
  appState->folderPlus = loadStateValue( "folderPlus" , "0").toInt();
  appState->folderIndex = loadStateValue( "folderIndex" , "0").toInt();
  appState->playMode = loadStateValue( "playMode", "0").toInt();
  appState->volume = loadStateValue( "volume", "50").toInt();
}

void DBHandler::saveAppState() 
{
  QSqlQuery q;
  q.exec(TRUNCATE_STATE_TABLE);
  q.exec(CREATE_STATE_TABLE);
  saveStateValue( "folderPath", appState->folderPath );
  saveStateValue( "folderPlus", QString::number(appState->folderPlus ? 1 : 0) );
  saveStateValue( "folderIndex", QString::number(appState->folderIndex) );
  saveStateValue( "playMode", QString::number(appState->playMode) );
  saveStateValue( "volume", QString::number(appState->volume) );
}

void DBHandler::populateDB(const QString& musicPath, const QString& videoPath, const QString& extensions) 
{    
	numFiles = 0;
  QSqlQuery q;
  q.exec(TRUNCATE_MUSIC_TABLE);
  q.exec(CREATE_MUSIC_TABLE);
  q.exec(CREATE_MUSIC_INDEX);
  subPopulate( musicPath, videoPath, extensions );
}

void DBHandler::subPopulate(const QString& musicPath, const QString& videoPath, const QString& extensions) 
{    
  if ( musicPath == QString::null )
	  return;

  QFileInfo info(musicPath);
  Tag tag;
  if (!info.isDir()) {
    tag.link(info.absFilePath());
    QString title = tag.getTitle();
    if (title == "Unknown") {
      title = info.fileName();
    }

    QSqlQuery query;
    query.prepare(INSERT_MUSIC_ITEM);
    query.bindValue(":key", info.dirPath(TRUE));
    query.bindValue(":artist", tag.getArtist());
    query.bindValue(":album", tag.getAlbum());
    query.bindValue(":title", title);
    query.bindValue(":genre", tag.getGenre());
    query.bindValue(":mrl", "file://" + info.absFilePath());
    
    query.exec();
    numFiles++;
    emit dbStatus(numFiles,title);
    return;
  }
      
  if (extensions.isNull()) {
	qWarning("no extensions specified");
	return;
  }
//    qWarning("exts: '%s'",extensions.latin1());
  QDir dir(musicPath,extensions.latin1(), QDir::DirsFirst);
  dir.setMatchAllDirs(true);
  if (!dir.isReadable())
  	return;
  
  QFileInfoList *entries = (QFileInfoList *)dir.entryInfoList();
  QFileInfo *fi;
  QString absPath;
  for ( fi = entries->first(); fi; fi = entries->next() ) {
    if (fi->fileName().at(0) != '.')
      subPopulate( fi->absFilePath(), videoPath, extensions );
  }
  return;
}

DBHandler::DBHandler(void) : valid(false)
{
  QString dbName("headunit.db");
  db = QSqlDatabase::addDatabase( DB_DRIVER );
  if (!db) {
    qWarning("Could not open SQLite driver");
    exit(1);
  }
  db->setDatabaseName( dbName );
  valid = db->open();
}

bool DBHandler::isEmpty(void)
{
  return (db->tables().empty());
}

bool DBHandler::loadMusicList(const QString& path, bool plus, MediaList& list)
{
  QString key = path;
  if (plus)
    key += "%";
  QSqlQuery q;
  q.prepare(QUERY_MUSIC_ITEM_BY_KEY);
  q.bindValue(":key", key);
  QString queryString = q.lastQuery();
  q.exec();
  list.clear();
  if (q.isActive()) {
    while (q.next()) {
      MediaItem mi(q.value(0).toInt(),
                   q.value(1).toString(),
                   q.value(2).toString(),
                   q.value(3).toString(),
                   q.value(4).toString(),
                   q.value(5).toString());
      list.add(mi);
    }
    return true;
  }
  return false;
}

QString DBHandler::getKeyForId(int id)
{
  QSqlQuery q;
  q.prepare(QUERY_MUSIC_ITEM_BY_ID);
  q.bindValue(":id", id);
  q.exec();
  if (q.isActive()) {
    if (q.next()) {
      QString key = q.value(6).toString();
      return key;
    }    
  }
  return QString::null;
}

MediaItem DBHandler::loadMediaItem(int id)
{
  QSqlQuery q;
  q.prepare(QUERY_MUSIC_ITEM_BY_ID);
  q.bindValue(":id", id);
  q.exec();
  if (q.isActive()) {
    if (q.next()) {
      MediaItem mi(q.value(0).toInt(),
                  q.value(1).toString(),
                  q.value(2).toString(),
                  q.value(3).toString(),
                  q.value(4).toString(),
                  q.value(5).toString());
      return mi;
    }    
  }
  MediaItem mi;
  return mi;
}

QString DBHandler::loadStateValue(const QString& var, const QString& def)
{
  QSqlQuery q;
  q.prepare(QUERY_STATE);
  q.bindValue(":var", var);
  q.exec();
  if (q.isActive()) {
    if (q.next()) {
      return q.value(0).toString();
    }
  }
  return def;
}

void DBHandler::saveStateValue(const QString& var, const QString& value)
{
  QSqlQuery q;
  q.prepare(INSERT_STATE);
  q.bindValue(":var", var);
  q.bindValue(":value", value);
  q.exec();
}
