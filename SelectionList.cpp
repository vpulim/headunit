#include "SelectionList.h"

SelectionList::SelectionList( QWidget *parent, const char *name )
  : QListBox(parent, name, Qt::WStyle_Customize|Qt::WStyle_NoBorderEx )
{
  setVScrollBarMode(AlwaysOff);
  setHScrollBarMode(AlwaysOff);
  setFont(QFont());
}

bool SelectionList::scrollUp()
{
  if (count() == 0)
    return false;
  if (currentItem() <= 0)
    setCurrentItem(0);
  else
    setCurrentItem(currentItem() - 1);
  return true;
}

bool SelectionList::scrollDown()
{
  if (count() == 0 || currentItem() + 1 >= count())
	return false;
  setCurrentItem(currentItem() + 1);
  return true;
}

bool SelectionList::scrollPageUp()
{
  int di = size().height()/(itemHeight+3);
  if (count() == 0)
    return false;
  if (currentItem() <= di)
    setCurrentItem(0);
  else
    setCurrentItem(currentItem() - di);
  return true;
}

bool SelectionList::scrollPageDown()
{
  int di = size().height()/(itemHeight+3);
  if (count() == 0 || currentItem() + 1 >= count())
	return false;
  if (currentItem() + di >= count())
    setCurrentItem(count()-1);
  else
    setCurrentItem(currentItem() + di);
  return true;
}

void SelectionList::fontChange(const QFont &oldFont)
{
  itemHeight = font().pointSize();
}
