#ifndef DBHANDLER_H
#define DBHANDLER_H

// File table
#define CREATE_MUSIC_TABLE "create table music (id integer primary key, key text, artist text, album text, title text, genre text, mrl text)"
#define CREATE_MUSIC_INDEX "create index key_index on music(key)"
#define TRUNCATE_MUSIC_TABLE "drop table music"
#define INSERT_MUSIC_ITEM "insert into music (key, artist, album, title, genre, mrl) VALUES (:key, :artist, :album, :title, :genre, :mrl)"
#define QUERY_MUSIC_ITEM_BY_MRL "select id, mrl, artist, album, title, genre, key from music where mrl = :mrl"
#define QUERY_MUSIC_ITEM_BY_KEY "select id, mrl, artist, album, title, genre, key from music where key like :key"
#define QUERY_MUSIC_ITEM_BY_ID "select id, mrl, artist, album, title, genre, key from music where id = :id"

// Playlist table
#define CREATE_PLAYLIST_TABLE "create table playlist (id integer primary key, mrl text)"
#define TRUNCATE_PLAYLIST_TABLE "drop table playlist"
#define INSERT_PLAYLIST "insert into playlist (mrl) values (:mrl)"
#define QUERY_PLAYLIST "select mrl from playlist"

// State table
#define CREATE_STATE_TABLE "create table state (var text primary key, value text)"
#define TRUNCATE_STATE_TABLE "drop table state"
#define INSERT_STATE "insert into state (var,value) values (:var,:value)"
#define QUERY_STATE "select value from state where var = :var"

#ifdef Q_WS_WIN
#define DB_DRIVER "QSQLITEX"
#else
#define DB_DRIVER "QSQLITE"
#endif

#include <qsqldatabase.h>

class MediaItem;
class MediaList;

class DBHandler : public QObject {
  Q_OBJECT
private:
    QSqlDatabase *db;
    int numFiles;
	int valid;
	void subPopulate(const QString& musicPath, const QString& videoPath, const QString& extList);
  QString loadStateValue(const QString& var, const QString& def = QString::null);
  QString loadStateValue(const char *var, const char *def = NULL) { return loadStateValue(QString(var), def == NULL ? QString::null : QString(def)); };
  void saveStateValue(const QString& var, const QString& value);
  void saveStateValue(const char *var, const char *value) { saveStateValue(QString(var), QString(value)); };
  void saveStateValue(const char *var, const QString& value) { saveStateValue(QString(var), value); };
public:
  DBHandler();
	bool isNull() { return valid; };
	bool isEmpty();
  bool loadMusicList(const QString& key, bool plus, MediaList& ml);
  QString getKeyForId(int id);
  MediaItem loadMediaItem(int id);
signals:
  void dbStatus(int,const QString&);
public slots:
	void populateDB(const QString& musicPath, const QString& videoPath, const QString& extList);
  void loadAppState();
  void saveAppState();
};

#endif
