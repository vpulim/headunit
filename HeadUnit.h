#ifndef HEADUNIT_H
#define HEADUNIT_H

extern class MenuScreen *menu;
extern class AudioPlayerScreen *audioPlayer;
extern class AudioBrowserScreen *audioBrowser;
extern class MediaPlayer *mediaPlayer;
extern class QSettings *settings;
extern class QSqlDatabase *db;

#ifdef Q_WS_WIN
#define DB_DRIVER "QSQLITEX"
#else
#define DB_DRIVER "QSQLITE"
#endif

#endif 
