#ifndef SCREEN_H
#define SCREEN_H

#include <qframe.h>

class FunctionScreen : public QFrame
{
 public:
  FunctionScreen(const char *name, QWidget *parent = 0);
  bool isNull() { return !valid; };
  void init() {};

 protected:
  bool valid; 
};

#endif // SCREEN_H
