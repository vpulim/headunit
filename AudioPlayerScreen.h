#ifndef AUDIOPLAYERSCREEN_H
#define AUDIOPLAYERSCREEN_H

#include "Screen.h"
#include "MediaList.h"

class Button;
class QLabel;
class SelectionList;

class AudioPlayerScreen : public FunctionScreen
{
  Q_OBJECT
 public:
  AudioPlayerScreen();
  void init();

 public slots:
  void endOfStreamReached();
  void loadFolder(QString& path, bool plus, int index);

 private slots:
  void play();
  void stop();
  void previous();
  void next();
  void updateInfo();

 private:
  enum {VISU, MODE, MIXER, MUTE, VOLDN, VOLUP, PREV, PLAY, NEXT, STOP, 
	EJECT, LIST, EXIT, PGDOWN, DOWN, UP, PGUP, NUM_BUTTONS};
  Button *buttons[NUM_BUTTONS];
  static const char *buttonKeys[NUM_BUTTONS];

  enum {TRACKNUMBER, TRACKNAME, PLAYLIST, VOLUME, STATUS, 
	CURRENTTRACKTIME, TRACKTIME, TIME, DATE, NUM_LABELS};
  QLabel *labels[NUM_LABELS];
  static const char *labelKeys[NUM_LABELS];

  SelectionList *listView;
  MediaList playList;
  static const char *slKey;

  QTimer *updateTimer;
};

#endif
