#include <qapplication.h>
#include <qframe.h>
#include "Screen.h"
#include "Skin.h"

FunctionScreen::FunctionScreen(const char *name, QWidget *parent) 
  : QFrame ( parent, name, 
	     Qt::WStyle_Customize | 
	     Qt::WStyle_NoBorderEx )
{
  Skin skin("menu.skin");
  if (skin.isNull()) {
    return;
  }
  skin.set(*this);
  move(0,0);
  valid = false;
}

void FunctionScreen::display()
{
  raise();
  show();
}

void FunctionScreen::hide()
{
  lower();
  QFrame::hide();
}

