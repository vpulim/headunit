#ifndef MEDIA_ITEM_H
#define MEDIA_ITEM_H

#include <qstring.h>

class MediaItem
{
 public:
  MediaItem();
  MediaItem(int id,
	    const QString &artist,
	    const QString &album,
	    const QString &title,
	    const QString &genre,
      const QString &mrl); 
  MediaItem &operator= (const MediaItem &m);
  int id() const { return _id; };
  QString artist() const { return _artist; };
  QString album() const { return _album; };
  QString title() const { return _title; };
  QString genre() const { return _genre; };
  QString mrl() const { return _mrl; };
  bool isNull() { return _mrl.isNull(); };

  static const MediaItem null;

 private:
  int _id;
  QString _artist;
  QString _album;
  QString _title;
  QString _genre;
  QString _mrl;
};

#endif
