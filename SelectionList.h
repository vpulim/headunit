#ifndef SELECTION_LIST_H
#define SELECTION_LIST_H

#include <qlistbox.h>

class SelectionList : public QListBox
{
  Q_OBJECT
 public:
  SelectionList( QWidget *parent, const char *name );
  void insertDir(QString dir);
  bool isDir(int index);
  QString dir(int index);

public slots:
  bool scrollUp();
  bool scrollDown();
  bool scrollPageUp();
  bool scrollPageDown();

 protected:
  void fontChange(const QFont &oldFont);

 private:
  int itemHeight;
};

#endif // BUTTON_H
