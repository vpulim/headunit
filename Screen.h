#ifndef SCREEN_H
#define SCREEN_H

#include <qframe.h>

class FunctionScreen : public QFrame
{
  Q_OBJECT
 public:
  FunctionScreen(const char *name, QWidget *parent = 0);
  bool isNull() { return !valid; };
  void init() {};

 public slots:
  void display();

 protected:
  bool valid; 
};

#endif // SCREEN_H
