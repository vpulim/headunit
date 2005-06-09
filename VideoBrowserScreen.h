#ifndef VIDEO_BROWSER_SCREEN_H
#define VIDEO_BROWSER_SCREEN_H

#include "Screen.h"
#include "AudioBrowserScreen.h"
#include "MediaList.h"

class Button;
class SelectionList;
class QLabel;

class VideoBrowserScreen : public AudioBrowserScreen
{
  Q_OBJECT
 public:
  VideoBrowserScreen(QWidget* parent = 0);
  void init();
  void initSkin();

 protected:
  enum {SELECT, BACK, BROWSE, EXIT, PGDOWN, DOWN, UP, PGUP, NUM_BUTTONS};
  Button *buttons[NUM_BUTTONS];
  static const char *buttonKeys[NUM_BUTTONS];
  static const char *slKey;
};

#endif
