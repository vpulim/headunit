#include <qapplication.h>
#include <qframe.h>
#include "Screen.h"
#include "Skin.h"

FunctionScreen::FunctionScreen(const char *name, QWidget *parent) 
  : QFrame ( parent, name, 
	     Qt::WStyle_Customize | 
	     Qt::WStyle_NoBorderEx )
{
//    Skin skin("wait.skin");
//    if (skin.isNull()) {
//      return;
//    }
//    skin.set(*this);
//  setSkin();
  move(0,0);
  valid = false;
}

void FunctionScreen::initSkin(const QString &skinFileName){
  //Skin skin("wait.skin");
  Skin skin(skinFileName);
  if (skin.isNull()) {
    return;
  }
  skin.set(*this);

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

