#ifndef MEDIA_PLAYER_H
#define MEDIA_PLAYER_H

#include <qsettings.h>
#include "Screen.h"
#include "MediaItem.h"

class QFrame;
class VisPanel;
class DVDPanel;
class VideoPanel;
extern class QSettings settings;

class MediaPlayer : public FunctionScreen
{
  Q_OBJECT
 public:
  MediaPlayer(QWidget* parent = 0);
  void init();
  bool isOpened() { return !openedItem.isNull(); };
  bool isPlaying() { return playState == PLAYING; };
  bool isPaused() { return playState == PAUSED; };
  bool isStopped() { return playState == STOPPED; };
  bool isVideo() {
    if (openedItem.isNull())
      return FALSE;
    return openedItem.fileName()
      .startsWith(settings.readEntry("headunit/videopath")
		  .replace(QChar('\\'), QChar('/')));
  };
  bool isAudio() { return !isVideo() && !isDVD(); };
  bool isDVD() { return openedItem.mrl().startsWith("dvd://"); };
  MediaItem &getOpened() { return openedItem; };
  bool getPosition(long *pos, long *len);
  bool setPosition(long pos);
  int getVolume() { return volume; };
  void setVolume(int volume);
  bool close(bool deleteAlso) {closeItem();return QWidget::close(deleteAlso);};
  void dvdPause();
  void dvdResume();
  bool dvdNextChapter();
  bool dvdPrevChapter();
  void dvdGetLocation(int *title, int *chapter, long *pos, long *len);

 public slots:
  void openItem(const MediaItem &m);
  void closeItem();
  void play();
  void pause();
  void stop();
  void volumeUp();
  void volumeDown();
  void volumeMute();
  void showAsVis();
  void showAsVideo();
  void showAsDVD();
  void display() { erase(); FunctionScreen::display(); };

 protected:
  void moveEvent ( QMoveEvent * );
  void resizeEvent ( QResizeEvent * );
  void mouseMoveEvent ( QMouseEvent * );
  void mouseReleaseEvent ( QMouseEvent * );
  void keyPressEvent ( QKeyEvent * );

 private slots:
  void cleanup();

 private:
  enum {PLAYING = 0, PAUSED, STOPPED};

  bool panel_on;
  MediaItem openedItem;
  int playState;
  VisPanel *visPanel;
  VideoPanel *videoPanel;
  DVDPanel *dvdPanel;
  FunctionScreen *activePanel;
  int volume;
  int mute;
  char buf[256];
};

#endif
