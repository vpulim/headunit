#ifndef DVDPANEL_H
#define DVDPANEL_H

#include "Panel.h"

class DVDPanel : public Panel
{
  Q_OBJECT
 public:
  DVDPanel(QWidget *parent);

 protected slots:
  void updateInfo();
  bool previous();
  bool next();
  void playpause();
};

#endif
