#ifndef MEDIA_PLAYER_H
#define MEDIA_PLAYER_H

#include "Screen.h"
#include "MediaItem.h"

class QFrame;

class MediaPlayer : public FunctionScreen
{
  Q_OBJECT
 public:
  MediaPlayer();
  void init();
  bool isOpened() { return !openedItem.isNull(); };
  bool isPlaying() { return playState == PLAYING; };
  bool isPaused() { return playState == PAUSED; };
  bool isStopped() { return playState == STOPPED; };
  MediaItem &getOpened() { return openedItem; };
  bool getPosition(long *pos, long *len);
  bool setPosition(long pos);
  int getVolume() { return volume; };
  void setVolume(int volume);
  bool close(bool deleteAlso) {closeItem();return QWidget::close(deleteAlso);};

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

 protected:
  void moveEvent ( QMoveEvent * );
  void resizeEvent ( QResizeEvent * );
  void mouseReleaseEvent ( QMouseEvent * );
  void keyPressEvent ( QKeyEvent * );

 private slots:
  void cleanup();

 private:
  enum {PLAYING, PAUSED, STOPPED};

  bool panel_on;
  MediaItem openedItem;
  int playState;
  FunctionScreen *visPanel;
  FunctionScreen *videoPanel;
  FunctionScreen *dvdPanel;
  FunctionScreen *activePanel;
  int volume;
  int mute;
  char buf[256];
};

#endif
