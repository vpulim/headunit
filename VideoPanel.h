#ifndef VIDEOPANEL_H
#define VIDEOPANEL_H

#include "Panel.h"
#include "MediaList.h"

class VideoPanel : public Panel
{
  Q_OBJECT
 public:
  VideoPanel(QWidget *parent);
  void init();

 public slots:
  void display();   
  void loadFolder(QString& path, bool plus, int index, long pos);
  void play();
  void stop();
  bool previous();
  bool next();

 private:
  MediaList playList;
};

#endif
