#include <qapplication.h>
#include <qsettings.h>
#include <qsqldatabase.h>
#include <qdir.h>
#include "HeadUnit.h"
#include "ConfigDialog.h"
#include "DBHandler.h"

void DBHandler::populateDB(QString& musicPath, QString& videoPath, QString& extensions) 
{    
    if ( musicPath == QString::null )
	return;

	numFiles = 0;

    QFileInfo info(musicPath);
    if (!info.isDir()) {
		QString mrl("file://" + info.absFilePath());
	
		QSqlQuery query;
		query.prepare(INSERT_MUSIC_ITEM);
		query.bindValue(":artist", "Unknown");
		query.bindValue(":album", "Unknown");
		query.bindValue(":title", info.fileName().latin1());
		query.bindValue(":genre", "Unknown");
		query.bindValue(":mrl", mrl.latin1());
	
//		qWarning(mrl);
		query.exec();
		numFiles++;
		emit dbStatus(numFiles,musicPath);
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

void DBHandler::subPopulate(QString& musicPath, QString& videoPath, QString& extensions) 
{    
    if ( musicPath == QString::null )
	return;

    QFileInfo info(musicPath);
    if (!info.isDir()) {
		QString mrl("file://" + info.absFilePath());
	
		QSqlQuery query;
		query.prepare(INSERT_MUSIC_ITEM);
		query.bindValue(":artist", "Unknown");
		query.bindValue(":album", "Unknown");
		query.bindValue(":title", info.fileName().latin1());
		query.bindValue(":genre", "Unknown");
		query.bindValue(":mrl", mrl.latin1());
	
//		qWarning(mrl);
		query.exec();
		numFiles++;
		emit dbStatus(numFiles,musicPath);
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

DBHandler::DBHandler(void)
{
  QString dbName("headunit.db");
  db = QSqlDatabase::addDatabase( DB_DRIVER );
  if (!db) {
    qWarning("Could not open SQLite driver");
    exit(1);
  }
  db->setDatabaseName( dbName );
  db->open();
}

bool DBHandler::isEmpty(void)
{
  return (db->tables().empty());
}
