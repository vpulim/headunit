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
  bool isPlaying() { return playing; };
  MediaItem &getOpened() { return openedItem; };
  bool getPosition(int *pos, int *len);
  int getVolume() { return volume; };

 public slots:
  void open(const MediaItem &m);
  void close();
  void play();
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
  bool panel_on;
  MediaItem openedItem;
  bool playing;
  FunctionScreen *visPanel;
  FunctionScreen *videoPanel;
  FunctionScreen *dvdPanel;
  FunctionScreen *activePanel;
  int volume;
  bool mute;
  char buf[256];
};

#endif
