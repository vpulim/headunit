#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include "Button.h"
#include "Screen.h"

class MenuScreen : public FunctionScreen
{
 public:
  MenuScreen();
  void init();

 private:
  enum { AUDIO, VIDEO, DVD, RADIO, LAUNCH, GPS, MUTE, VOLDN, VOLUP, MIXER, 
	 EXIT, GAMMA, APPEAR, NUM_BUTTONS }; 
  Button *buttons[NUM_BUTTONS];
  static const char *keys[NUM_BUTTONS];
};

#endif // MENUSCREEN_H
