#ifndef XINE_VIDEO_H
#define XINE_VIDEO_H

#include "Screen.h"

class QFrame;

class XineVideo : public FunctionScreen
{
  Q_OBJECT
 public:
  XineVideo();
  void init();
  void redirectOutput(QWidget *w);
  void restoreOutput();
  bool isOpened() { return !openedMRL.isNull(); };
  bool isPlaying() { return playing; };
  const char *getArtist();
  const char *getAlbum();
  const char *getTitle();
  const char *getGenre();
  bool getPosition(int *pos, int *len);
  int getVolume() { return volume; };

 public slots:
  void open(const QString &mrl);
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
  void endStream();
  void cleanup();

 private:
  bool panel_on;
  QString openedMRL;
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
