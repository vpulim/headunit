#ifndef MEDIALIST_H
#define MEDIALIST_H

#include "MediaItem.h"

#include <qvaluevector.h>

class MediaList
{
 public:
  void add(MediaItem& item) { items.append(item); };
  void clear() { items.clear(); };
  int size() { return items.size(); };
  MediaItem operator[] ( int i ) const { return items[i]; };

 private:
  QValueVector<MediaItem> items;
};

#endif
