#include <qapplication.h>
#include <qsettings.h>
#include <qsqldatabase.h>
#include <qdir.h>
#include <qbuffer.h>
#include <qpixmap.h>
#include <qwmatrix.h>
#include "HeadUnit.h"
#include "ConfigDialog.h"
#include "DBHandler.h"
#include "ApplicationState.h"
#include "MediaList.h"
#include "Tag.h"

int sqlite_encode_binary(const unsigned char *in, int n, unsigned char *out);
int sqlite_decode_binary(const unsigned char *in, unsigned char *out);

DBHandler::DBHandler() : valid(false)
{
  QString dbName("headunit.db");
  db = QSqlDatabase::addDatabase( DB_DRIVER );
  if (!db) {
    qWarning("Could not open SQLite driver");
    exit(1);
  }
  db->setDatabaseName( dbName );
  valid = db->open();
  dataBuffer.resize(DATABUFFER_SIZE, QGArray::SpeedOptim);
}

DBHandler::~DBHandler()
{
}

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
  q.exec(TRUNCATE_ALBUMART_TABLE);
  q.exec(CREATE_ALBUMART_TABLE);
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
  QString extPlusJPG = extensions + " *.jpg";
  QDir dir(musicPath,extPlusJPG.latin1(), QDir::DirsFirst);
  dir.setMatchAllDirs(true);
  if (!dir.isReadable())
  	return;
  
  QFileInfoList *entries = (QFileInfoList *)dir.entryInfoList();
  QFileInfo *fi;
  for ( fi = entries->first(); fi; fi = entries->next() ) {
    if (fi->fileName().at(0) != '.') {
      if (fi->fileName().endsWith(".jpg", FALSE)) {
	QString key = fi->dirPath(TRUE);
	QPixmap image(fi->absFilePath());
	saveAlbumArt(key, image);
      }
      else
	subPopulate( fi->absFilePath(), videoPath, extensions );
    }
  }
  return;
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

QPixmap DBHandler::loadAlbumArt(const QString& key) 
{
  QSqlQuery q;
  q.prepare(QUERY_ALBUMART);
  q.bindValue(":key", key);
  q.exec();  
  QPixmap image;
  if (q.isActive()) {
    if (q.next()) {
      QString blob = q.value(0).toString();
      dataBuffer.resize(blob.length(), QGArray::SpeedOptim);
      int decodedSize = sqlite_decode_binary((uchar *)blob.latin1(),
					     (uchar *)dataBuffer.data());
      dataBuffer.truncate(decodedSize);
      image.loadFromData(dataBuffer);
      if (image.isNull())
	qWarning("error loading album art for %s", key.latin1());      
    }
  }
  return image;
}

void DBHandler::saveAlbumArt(QString& key, QPixmap& image)
{
  QByteArray ba(DATABUFFER_SIZE);
  QBuffer buffer( ba );
  buffer.open( IO_WriteOnly );
  double scalew = 200.0/image.width();
  double scaleh = 200.0/image.height();
  QWMatrix m;
  m.scale(scalew,scaleh);
  QPixmap scaled = image.xForm(m);
  scaled.save( &buffer, "JPEG" );
  int size = ba.size();
  dataBuffer.resize((uint)(size*1.2), QGArray::SpeedOptim);
  uint encodedSize = sqlite_encode_binary((uchar *)ba.data(), ba.size(), 
					  (uchar *)dataBuffer.data());
  dataBuffer.truncate(encodedSize);
  QSqlQuery q;
  q.prepare(INSERT_ALBUMART);
  q.bindValue(":key", key);
  q.bindValue(":image", QString(dataBuffer));
  q.exec();  
}

// The following functions were copied out of the encode.c file in the sqlite
// source tree.

int sqlite_encode_binary(const unsigned char *in, int n, unsigned char *out){
  int i, j, e = 0, m;
  unsigned char x;
  int cnt[256];
  if( n<=0 ){
    if( out ){
      out[0] = 'x';
      out[1] = 0;
    }
    return 1;
  }
  memset(cnt, 0, sizeof(cnt));
  for(i=n-1; i>=0; i--){ cnt[in[i]]++; }
  m = n;
  for(i=1; i<256; i++){
    int sum;
    if( i=='\'' ) continue;
    sum = cnt[i] + cnt[(i+1)&0xff] + cnt[(i+'\'')&0xff];
    if( sum<m ){
      m = sum;
      e = i;
      if( m==0 ) break;
    }
  }
  if( out==0 ){
    return n+m+1;
  }
  out[0] = e;
  j = 1;
  for(i=0; i<n; i++){
    x = in[i] - e;
    if( x==0 || x==1 || x=='\''){
      out[j++] = 1;
      x++;
    }
    out[j++] = x;
  }
  out[j] = 0;
  assert( j==n+m+1 );
  return j;
}

/*
** Decode the string "in" into binary data and write it into "out".
** This routine reverses the encoding created by sqlite_encode_binary().
** The output will always be a few bytes less than the input.  The number
** of bytes of output is returned.  If the input is not a well-formed
** encoding, -1 is returned.
**
** The "in" and "out" parameters may point to the same buffer in order
** to decode a string in place.
*/
int sqlite_decode_binary(const unsigned char *in, unsigned char *out){
  int i, e;
  unsigned char c;
  e = *(in++);
  i = 0;
  while( (c = *(in++))!=0 ){
    if( c==1 ){
      c = *(in++) - 1;
    }
    out[i++] = c + e;
  }
  return i;
}
