#include <qapplication.h>
#include <qsettings.h>
#include <qsqldatabase.h>
#include <qdir.h>
#include "HeadUnit.h"
#include "ConfigDialog.h"
#include "DBHandler.h"

#define  NUM_EXTENSIONS 4

static const QString default_extensions[]={"mp3","wma","avi","mpg"};

void DBHandler::populateDB(QString& path) 
{    
    if ( path == QString::null )
	return;

    QFileInfo info(path);
    if (!info.isDir()) {
	QString mrl("file://" + info.absFilePath());
	
	QSqlQuery query;
	query.prepare(INSERT_MUSIC_ITEM);
	query.bindValue(":artist", "Unknown");
	query.bindValue(":album", "Unknown");
	query.bindValue(":title", info.fileName().latin1());
	query.bindValue(":genre", "Unknown");
	query.bindValue(":mrl", mrl.latin1());
	
	qWarning(mrl);
	query.exec();
	numFiles+=1;
	emit dbStatus(numFiles,path);
	return;
    }
    
    QStringList list = settings.entryList( "/headunit/extensions" );
    QStringList::Iterator it = list.begin();
    QString extensions;
    settings.beginGroup("/headunit/extensions");
    while( it != list.end() ) {
	extensions+="*."+settings.readEntry((*it).latin1())+" ";
	it++;
    }
    settings.endGroup();
    
    if (extensions.isNull()) {
	qWarning("extensions is null, populating defaults");
	for (unsigned char i=0;i<NUM_EXTENSIONS;++i) {
	    settings.writeEntry(QString("/headunit/extensions/")+QString::number(i),default_extensions[i]);
	    extensions+="*."+default_extensions[i]+" ";
	}
    }
    qWarning("exts: '%s'",extensions.latin1());
    QDir dir(path,extensions.latin1(), QDir::DirsFirst);
//    QDir dir(path, "*.mp3 *.wma *.avi *.mpg", QDir::DirsFirst);
    dir.setMatchAllDirs(true);
    if (!dir.isReadable())
	return;
  
  QFileInfoList *entries = (QFileInfoList *)dir.entryInfoList();
  QFileInfo *fi;
  QString absPath;
  for ( fi = entries->first(); fi; fi = entries->next() ) {
    if (fi->fileName().at(0) != '.')
      populateDB( fi->absFilePath() );
  }
  return;
}

void DBHandler::resetDB(void)
{
    db->exec( DUMP_MUSIC_TABLE );
    numFiles=0;
    populateDB(settings.readEntry(QString("headunit/mediapath")));
}

DBHandler::DBHandler(void)
{
  QString dbName("headunit.db");
  if (dbName.isNull()) {
    qWarning("no database defined");
    exit(1);
  }
  db = QSqlDatabase::addDatabase( DB_DRIVER );
  if (!db) {
    qWarning("Could not open SQLite driver");
    exit(1);
  }
  db->setDatabaseName( dbName );
  db->open();
  
  if (db->tables().empty()) {
    qWarning("Initializing database: %s", dbName.latin1());
    db->exec( CREATE_MUSIC_TABLE );
    populateDB(settings.readEntry("headunit/mediapath", "./"));
  }
}
