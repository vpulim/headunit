#ifndef AUDIO_BROWSER_SCREEN_H
#define AUDIO_BROWSER_SCREEN_H

#include "Screen.h"

class Button;
class SelectionList;

class AudioBrowserScreen : public FunctionScreen
{
  Q_OBJECT
 public:
  AudioBrowserScreen();
  void init();

 private slots:
  void highlight(int);  
  void select(int);
  void selectHighlighted();

 signals:
  void fileSelected(const QString &file);

 private:
  void setDir(const QString &path);

  enum {SELECT, PLUS, BACK, BROWSE, EXIT, PGDOWN, DOWN, UP, PGUP, NUM_BUTTONS};
  Button *buttons[NUM_BUTTONS];
  SelectionList *fileList;
  static const char *buttonKeys[NUM_BUTTONS];
  static const char *slKey;
  QString audioFileFilter;
  QString rootDir;
  QString currDir;
};

#endif
