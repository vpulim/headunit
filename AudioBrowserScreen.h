#ifndef AUDIO_BROWSER_SCREEN_H
#define AUDIO_BROWSER_SCREEN_H

#include "Screen.h"
#include "MediaList.h"

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
  void selectFolder();
  void selectFolderPlus();
  void backFolder();
  void browseFolder();

 signals:
  void folderSelected(QString &path, bool plus, int index);

 private:
  void setDir(const QString &path);
  void folderSelect(bool plus);

  enum {SELECT, PLUS, BACK, BROWSE, EXIT, PGDOWN, DOWN, UP, PGUP, NUM_BUTTONS};
  Button *buttons[NUM_BUTTONS];
  MediaList musicList;
  SelectionList *listView;
  static const char *buttonKeys[NUM_BUTTONS];
  static const char *slKey;
  QString rootDir;
  QString currDir;
};

#endif
