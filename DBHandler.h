#ifndef DBHANDLER_H
#define DBHANDLER_H

#define CREATE_MUSIC_TABLE "create table music (id integer primary key, artist text, album text, title text, genre text, mrl text)"
#define INSERT_MUSIC_ITEM "insert into music (artist, album, title, genre, mrl) VALUES (:artist, :album, :title, :genre, :mrl)"
#define DUMP_MUSIC_TABLE "truncate table music"

#ifdef Q_WS_WIN
#define DB_DRIVER "QSQLITEX"
#else
#define DB_DRIVER "QSQLITE"
#endif

#include <qsqldatabase.h>

class DBHandler : public QObject {
  Q_OBJECT
private:
    QSqlDatabase *db;
    int numFiles;
public:
    DBHandler();
    void populateDB(QString path);
    void resetDB(void);
signals:
    void dbStatus(int,QString);
};
#endif
