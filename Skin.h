#ifndef SKIN_H
#define SKIN_H

#include <qframe.h>
#include <qlabel.h>
#include "Button.h"
#include "SelectionList.h"

typedef QValueList<QStringList> SkinItemList;

class Skin
{
 public:
  Skin( const QString &skinFileName);
  ~Skin();
  bool isNull() { return items.empty(); }
  void set(QFrame &f);
  Button *getButton(const char *code, QWidget &parent);
  QLabel *getLabel(const char *code, QWidget &parent);
  SelectionList *getSelectionList(const char *key, QWidget &parent);
  
 private:
  SkinItemList items;
  QImage *emptyImage;
  QImage *offImage;
  QImage *onImage;
  QImage *downImage;
};


#endif // SKIN_H

