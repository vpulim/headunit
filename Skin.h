#ifndef SKIN_H
#define SKIN_H

#include <qframe.h>
#include <qlabel.h>
#include "Button.h"
#include "SelectionList.h"

typedef QMap<QString, QStringList> SkinMap;

class Skin
{
 public:
  Skin( const QString &skinfName);
  ~Skin();
  bool isNull() { return map.empty(); }
  void set(QFrame &f);
  Button *getButton(const char *key, QWidget &parent);
  QLabel *getLabel(const char *key, QWidget &parent);
  SelectionList *getSelectionList(const char *key, QWidget &parent);
  
 private:
  SkinMap map;
  QImage *emptyImage;
  QImage *offImage;
  QImage *onImage;
  QImage *downImage;
};


#endif // SKIN_H

