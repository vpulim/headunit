#include "SelectionList.h"

SelectionList::SelectionList( QWidget *parent, const char *name )
  : QListBox(parent, name, Qt::WStyle_Customize|Qt::WStyle_NoBorderEx )
{
  setVScrollBarMode(AlwaysOff);
  setHScrollBarMode(AlwaysOff);
  setFont(QFont());
}

void SelectionList::scrollUp()
{
  if (count() == 0)
    return;
  if (currentItem() <= 0)
    setCurrentItem(0);
  else
    setCurrentItem(currentItem() - 1);
}

void SelectionList::scrollDown()
{
  if (count() == 0)
    return;
  setCurrentItem(currentItem() + 1);
}

void SelectionList::scrollPageUp()
{
  int di = size().height()/(itemHeight+3);
  if (count() == 0)
    return;
  if (currentItem() <= di)
    setCurrentItem(0);
  else
    setCurrentItem(currentItem() - di);
}

void SelectionList::scrollPageDown()
{
  int di = size().height()/(itemHeight+3);
  if (count() == 0)
    return;
  if (currentItem() + di >= count())
    setCurrentItem(count()-1);
  else
    setCurrentItem(currentItem() + di);
}

void SelectionList::fontChange(const QFont &oldFont)
{
  itemHeight = font().pointSize();
}
