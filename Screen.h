#ifndef SCREEN_H
#define SCREEN_H

#include <qframe.h>
#include <Skin.h>

class FunctionScreen : public QFrame
{
  Q_OBJECT
 public:
  FunctionScreen(const char *name, QWidget *parent = 0);
  bool isNull() { return !valid; };
  void init() {};
  virtual void initSkin(const QString &skinFileName);

 public slots:
  void display();
  void hide();

 protected:
  bool valid; 
};

#endif // SCREEN_H
