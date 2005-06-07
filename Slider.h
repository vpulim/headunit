#ifndef SLIDER_H
#define SLIDER_H

#include <qlabel.h>

class Slider : public QLabel
{
 public:
  Slider( QWidget *parent, const char *name, Qt::Orientation orientation, int max, int barHeight );
  void drawContents( QPainter *paint );
  void setBarPixmap( const QPixmap &pixmap );
  void setOrientation(Qt::Orientation o) { orientation = o; };
  Qt::Orientation getOrientation() { return orientation; };
  void setValue(int v);
  int getValue() { return value; };
  void setMaxValue(int max);
  int getMaxValue() { return maxValue; };
  void setBarHeight(int h) { barHeight = h; };
  int getBarHeight() { return barHeight; };


 private:
  QPixmap *barpixmap;
  Qt::Orientation orientation;
  int maxValue;
  int value;
  int barHeight;
  
};

#endif // SLIDER_H
