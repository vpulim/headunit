#include "qpushbutton.h"
#include "qdialog.h"
#include "qfontmetrics.h"
#include "qpainter.h"
#include "qdrawutil.h"
#include "qpixmap.h"
#include "qbitmap.h"
#include "qpopupmenu.h"
#include "qguardedptr.h"
#include "qapplication.h"
#include "qtoolbar.h"
#include "qstyle.h"
#include "qaccessible.h"
#include "Button.h"

Button::Button( QWidget *parent, const char *name ) 
  : QPushButton( parent, name )
{
  onpixmap = offpixmap = downpixmap = NULL;
}

void Button::setOffPixmap( const QPixmap &pixmap )
{
  offpixmap = new QPixmap( pixmap );
  Q_CHECK_PTR( offpixmap );
  update();
  updateGeometry();
}

void Button::setOnPixmap( const QPixmap &pixmap )
{
  onpixmap = new QPixmap( pixmap );
  Q_CHECK_PTR( onpixmap );
  update();
  updateGeometry();
}

void Button::setDownPixmap( const QPixmap &pixmap )
{
  downpixmap = new QPixmap( pixmap );
  Q_CHECK_PTR( downpixmap );
  update();
  updateGeometry();
}

void Button::drawButton( QPainter *paint )
{
  if ((hasFocus() || isOn()) && onpixmap)
    paint->drawPixmap(0, 0, *onpixmap);
  else if (isDown() && downpixmap)
    paint->drawPixmap(0, 0, *downpixmap);
  else if (offpixmap)
    paint->drawPixmap(0, 0, *offpixmap);
}
