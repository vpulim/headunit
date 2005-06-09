#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include "Screen.h"

class Button;
class QLabel;
class Slider;

class MenuScreen : public FunctionScreen
{
  Q_OBJECT
 public:
  MenuScreen(QWidget* parent = 0);
  void init();
  virtual void initSkin();

 private slots:
  void updateInfo();
  void updateVolume();

 private:
  enum { AUDIO, VIDEO, DVD, RADIO, LAUNCH, GPS, MUTE, VOLDN, VOLUP, MIXER, 
	 APPEAR, EXIT, GAMMA, NUM_BUTTONS }; 
  Button *buttons[NUM_BUTTONS];
  static const char *keys[NUM_BUTTONS];

  enum {TIME, DATE, NUM_LABELS};
  QLabel *labels[NUM_LABELS];
  static const char *labelKeys[NUM_LABELS];

  enum {MASTER, NUM_SLIDERS};
  Slider *sliders[NUM_SLIDERS];
  static const char *sliderKeys[NUM_SLIDERS];

  QTimer *updateTimer;
};

#endif // MENUSCREEN_H
