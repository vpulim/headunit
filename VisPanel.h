#ifndef VISPANEL_H
#define VISPANEL_H

#include "Panel.h"

class VisPanel : public Panel
{
  Q_OBJECT
 public:
  VisPanel(QWidget *parent);

 protected slots:
  bool previous();
  bool next();
};

#endif
