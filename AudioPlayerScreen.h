#ifndef AUDIOPLAYERSCREEN_H
#define AUDIOPLAYERSCREEN_H

#include <qdatetime.h>
#include "Screen.h"
#include "MediaList.h"

#define UPDATE_DELAY 100
#define SLOW_SEEK_DELAY 400
#define SLOW_SEEK_MSECS 1000
#define MED_SEEK_DELAY 4000
#define MED_SEEK_MSECS 5000
#define FAST_SEEK_DELAY 10000
#define FAST_SEEK_MSECS 10000

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
  void playpause();
  void stop();
  bool previous();
  bool next();
  void nextPressed();
  void nextReleased();
  void prevPressed();
  void prevReleased();
  void updateInfo();
  void changeMode();

 private:
  enum {VISU, MODE, MIXER, MUTE, VOLDN, VOLUP, PREV, PLAY, NEXT, STOP, 
	EJECT, LIST, EXIT, PGDOWN, DOWN, UP, PGUP, NUM_BUTTONS};
  Button *buttons[NUM_BUTTONS];
  static const char *buttonKeys[NUM_BUTTONS];

  enum {TRACKNUMBER, TRACKNAME, PLAYLIST, VOLUME, STATUS, 
	CURRENTTRACKTIME, TRACKTIME, TIME, DATE, NUM_LABELS};
  QLabel *labels[NUM_LABELS];
  static const char *labelKeys[NUM_LABELS];

  enum {ISHUFFLE, IREPEAT, IMODE, NUM_INDICATORS};
  QLabel *indicators[NUM_INDICATORS];
  static const char *indicatorKeys[NUM_INDICATORS];

  enum {MODE_NORMAL, MODE_REPEAT, MODE_SHUFFLE, MODE_REPEATSHUFFLE, NUM_MODES};

  SelectionList *listView;
  MediaList playList;
  QValueVector<int> shuffleList;
  int shufflePos;
  static const char *slKey;
  QTime nextPressTime;
  bool nextHeldLong;   // used to prevent int overflow
  QTime prevPressTime;
  bool prevHeldLong;   // used to prevent int overflow
  int playMode;

  QTimer *updateTimer;
};

#endif
