#include <qapplication.h>
#include <qframe.h>
#include "Screen.h"

FunctionScreen::FunctionScreen(const char *name, QWidget *parent) 
  : QFrame ( parent, name, 
	     Qt::WStyle_Customize | 
	     Qt::WStyle_NoBorderEx )
{
  resize(800,480);
  move(0,0);
  valid = false;
}

void FunctionScreen::display()
{
  raise();
  show();
}