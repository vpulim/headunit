#ifndef PANEL_H
#define PANEL_H

#include "Screen.h"

class Button;
class QLabel;
class QTimer;

class Panel : public FunctionScreen
{
  Q_OBJECT
 public:
  Panel(QWidget *parent);
  
 protected slots:
  void updateInfo();

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
};

#endif