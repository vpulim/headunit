#ifndef XINEPANEL_H
#define XINEPANEL_H

#include "Screen.h"

class Button;
class QLabel;
class QTimer;

class XinePanel : public FunctionScreen
{
  Q_OBJECT
 public:
  XinePanel(QWidget *parent);
  
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
