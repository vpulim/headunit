#ifndef AUDIOPLAYERSCREEN_H
#define AUDIOPLAYERSCREEN_H

#include "Screen.h"

class Button;
class QLabel;
class SelectionList;

class AudioPlayerScreen : public FunctionScreen
{
  Q_OBJECT
 public:
  AudioPlayerScreen();
  void init();
  void addMRLToPlayList(const QString &file);

 public slots:
  void addFileToPlayList(const QString &file);

 private slots:
  void play();
  void stop();
  void previous();
  void next();
  void savePlayList();
  void updateInfo();

 private:
  void loadPlayList();

  enum {VISU, MODE, MIXER, MUTE, VOLDN, VOLUP, PREV, PLAY, NEXT, STOP, 
	EJECT, LIST, EXIT, PGDOWN, DOWN, UP, PGUP, NUM_BUTTONS};
  Button *buttons[NUM_BUTTONS];
  static const char *buttonKeys[NUM_BUTTONS];

  enum {TRACKNUMBER, TRACKNAME, PLAYLIST, VOLUME, STATUS, 
	CURRENTTRACKTIME, TRACKTIME, TIME, DATE, NUM_LABELS};
  QLabel *labels[NUM_LABELS];
  static const char *labelKeys[NUM_LABELS];

  SelectionList *playList;
  static const char *slKey;

  QTimer *updateTimer;
};

#endif
