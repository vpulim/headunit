#ifndef BUTTON_H
#define BUTTON_H

#include <qpushbutton.h>

class Button : public QPushButton
{
 public:
  Button( QWidget *parent, const char *name );
  void drawButton( QPainter *paint );
  void setOffPixmap( const QPixmap &pixmap );
  void setOnPixmap( const QPixmap &pixmap );
  void setDownPixmap( const QPixmap &pixmap );
  
 private:
  QPixmap *offpixmap;
  QPixmap *onpixmap;
  QPixmap *downpixmap;
};

#endif // BUTTON_H
