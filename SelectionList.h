#ifndef SELECTION_LIST_H
#define SELECTION_LIST_H

#include <qlistbox.h>

class SelectionList : public QListBox
{
  Q_OBJECT
 public:
  SelectionList( QWidget *parent, const char *name );

 public slots:
  void scrollUp();
  void scrollDown();
  void scrollPageUp();
  void scrollPageDown();

 protected:
  void fontChange(const QFont &oldFont);

 private:
  int itemHeight;
};

#endif // BUTTON_H
