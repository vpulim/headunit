#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include "Screen.h"

class Button;
class QLabel;

class MenuScreen : public FunctionScreen
{
  Q_OBJECT
 public:
  MenuScreen();
  void init();

 private slots:
  void updateInfo();

 private:
  enum { AUDIO, VIDEO, DVD, RADIO, LAUNCH, GPS, MUTE, VOLDN, VOLUP, MIXER, 
	 APPEAR, EXIT, GAMMA, NUM_BUTTONS }; 
  Button *buttons[NUM_BUTTONS];
  static const char *keys[NUM_BUTTONS];

  enum {TIME, DATE, NUM_LABELS};
  QLabel *labels[NUM_LABELS];
  static const char *labelKeys[NUM_LABELS];

  QTimer *updateTimer;
};

#endif // MENUSCREEN_H
