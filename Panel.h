#ifndef PANEL_H
#define PANEL_H

#include <qdatetime.h>
#include "Screen.h"

#ifndef UPDATE_DELAY
#define UPDATE_DELAY 100
#endif

#define SLOW_SEEK_DELAY 400
#define SLOW_SEEK_MSECS 1000
#define MED_SEEK_DELAY 4000
#define MED_SEEK_MSECS 5000
#define FAST_SEEK_DELAY 10000
#define FAST_SEEK_MSECS 10000

class Button;
class QLabel;
class QTimer;

class Panel : public FunctionScreen
{
  Q_OBJECT
 public:
  Panel(QWidget *parent);
  
 protected slots:
  virtual void updateInfo();
  virtual bool previous() = 0;
  virtual bool next() = 0;
  void nextPressed();
  void nextReleased();
  void prevPressed();
  void prevReleased();

 protected:
  void loadSkin();

  enum { MUTE, VOLDN, VOLUP, ZOOM, LIST, MIXER, PREV, STOP, PLAY, NEXT,
	 UP, DOWN, EXIT, NUM_BUTTONS }; 
  Button *buttons[NUM_BUTTONS];
  static const char *keys[NUM_BUTTONS];

  enum {TRACKNUMBER, TRACKNAME, VOLUME, STATUS, 
	CURRENTTRACKTIME, TRACKTIME, TIME, DATE, NUM_LABELS};
  QLabel *labels[NUM_LABELS];
  static const char *labelKeys[NUM_LABELS];

  QTimer *updateTimer;
  QTime nextPressTime;
  bool nextHeldLong;   // used to prevent int overflow
  QTime prevPressTime;
  bool prevHeldLong;   // used to prevent int overflow
  long diff; // hack to get around a problem with Xine
};

#endif
