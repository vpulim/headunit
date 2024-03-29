#ifndef HEADUNIT_H
#define HEADUNIT_H

#include <qapplication.h>
#include <qsettings.h>

extern class MenuScreen *menu;
extern class AudioPlayerScreen *audioPlayer;
extern class AudioBrowserScreen *audioBrowser;
extern class VideoBrowserScreen *videoBrowser;
extern class MediaPlayer *mediaPlayer;
extern class SkinBrowserScreen *skinBrowser;
extern class ConfigDialog *configDialog;
extern class DBHandler *dbHandler;
extern class ApplicationState *appState;
extern class QSettings settings;
	
QString getMediaPath(void);
QString askMediaPath(void);
void setMediaPath(QString path);
int initializeGui();
void destroyGui();
void refreshGui();
#endif 
