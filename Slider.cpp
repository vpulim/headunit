#include "qlabel.h"
//#include "qdialog.h"
//#include "qfontmetrics.h"
#include "qpainter.h"
//#include "qdrawutil.h"
#include "qpixmap.h"
#include "qwmatrix.h"
//#include "qbitmap.h"
//#include "qpopupmenu.h"
//#include "qguardedptr.h"
#include "qapplication.h"
//#include "qtoolbar.h"
//#include "qstyle.h"
//#include "qaccessible.h"
#include "Slider.h"

Slider::Slider( QWidget *parent, const char *name,  Qt::Orientation o, int max, int h, int w )
  : QLabel( parent, name )
{
  barpixmap = NULL;
  orientation = o;
  maxValue = max;
  value = 0;
  barHeight = h;
  barWidth = w;
  
  
      setBackgroundOrigin(AncestorOrigin);
      setPaletteBackgroundPixmap(*parentWidget()->
                                  paletteBackgroundPixmap());
}

void Slider::setBarPixmap( const QPixmap &pixmap )
{
  barpixmap = new QPixmap( pixmap );
  Q_CHECK_PTR( barpixmap );
  update();
  updateGeometry();
}

void Slider::drawContents( QPainter *paint )
{

/* if ((parentWidget()) &&
            (parentWidget()->paletteBackgroundPixmap()) &&
            (!parentWidget()->paletteBackgroundPixmap()->isNull()))
   {*/
      // If there is no background image specified and our
      // parent widget has one, we must set this background pixmap as our
      // and set properly our background origin in order to have the
      // correct transparence simulation.
//       setBackgroundOrigin(AncestorOrigin);
//       setPaletteBackgroundPixmap(*parentWidget()->
//                                   paletteBackgroundPixmap());
//    }
   
  if (orientation == Qt::Horizontal) {
    int x = (width() - barpixmap->width())*getValue()/getMaxValue();
    QWMatrix m;
    m.scale( 1, getBarHeight()/(double)barpixmap->height() );
    QPixmap scaled = barpixmap->xForm(m);
    
    bitBlt(paint->device(), x, 0, &scaled,
 	   0, 0, scaled.width(), scaled.height(), Qt::CopyROP, TRUE);
  } else {
    int y = height() - ((height() - getBarHeight())*getValue()/getMaxValue()) - getBarHeight() ;
    QWMatrix m;
    m.scale( getBarWidth()/(double)barpixmap->width(), getBarHeight()/(double)barpixmap->height() );
    QPixmap scaled = barpixmap->xForm(m);
    bitBlt(paint->device(), 0, y, &scaled, 
 	   0, 0, scaled.width(), scaled.height(), Qt::CopyROP, TRUE);
  }

//   if ((hasFocus() || isOn()) && onpixmap)
//     //    paint->drawPixmap(0, 0, *onpixmap);
//     bitBlt(paint->device(), 0, 0, onpixmap, 
// 	   0, 0, onpixmap->width(), onpixmap->height(), Qt::CopyROP, TRUE);
//   else if (isDown() && downpixmap)
//     //    paint->drawPixmap(0, 0, *downpixmap);
//     bitBlt(paint->device(), 0, 0, downpixmap, 
// 	   0, 0, downpixmap->width(), downpixmap->height(), Qt::CopyROP, TRUE);
//   else if (offpixmap)
//     //    paint->drawPixmap(0, 0, *offpixmap);
//     bitBlt(paint->device(), 0, 0, offpixmap, 
// 	   0, 0, offpixmap->width(), offpixmap->height(), Qt::CopyROP, TRUE);
}

void Slider::setMaxValue(int max) {
  maxValue = max;
  drawContents(new QPainter(this));
}

void Slider::setValue(int v) {
  value = v;
  repaint();
}

