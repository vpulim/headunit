#ifndef SKIN_BROWSER_SCREEN_H
#define SKIN_BROWSER_SCREEN_H

#include "Screen.h"
//#include "MediaList.h"

class Button;
class SelectionList;
class QLabel;
class QDir;

class SkinBrowserScreen : public FunctionScreen
{
  Q_OBJECT
 public:
  SkinBrowserScreen(QWidget* parent = 0);
  SkinBrowserScreen(QString name, QWidget* parent = 0)
    :FunctionScreen(name, parent) {};
  void init();

 private slots:
  void highlight(int);  
  void select();
  //void setSkin();
  //void selectFolderPlus();
  //void backFolder();
  //void browseFolder();

 signals:
  void folderSelected(QString &path, bool plus, int index, long pos);

 protected:
  void setDir();
  //void folderSelect(bool plus);
  void displaySkin(const QString& key);

  enum {SELECT, EXIT, PGDOWN, DOWN, UP, PGUP, NUM_BUTTONS};
  Button *buttons[NUM_BUTTONS];
  QLabel *skinPreview;
  //MediaList musicList;
  SelectionList *listView;
  static const char *buttonKeys[NUM_BUTTONS];
  static const char *slKey;
  QString rootDir;
  QString currDir;
};

#endif // SKIN_BROWSER_SCREEN_H
