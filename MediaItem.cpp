#include <qstring.h>
#include "MediaItem.h"

const MediaItem MediaItem::null;

MediaItem::MediaItem()
{
  _id = -1;
  _mrl = QString::null;
  _artist = QString::null;
  _album = QString::null;
  _title = QString::null;
  _genre = QString::null;
}

MediaItem::MediaItem(int id,
                     const QString &mrl, 
                     const QString &artist,
                     const QString &album,
                     const QString &title,
                     const QString &genre)
{
  _mrl = mrl;
  _artist = artist;
  _album = album;
  _title = title;
  _genre = genre;
  _id = id;
}

MediaItem &MediaItem::operator= (const MediaItem &m)
{
  _id = m._id;
	_mrl = m._mrl;
	_artist = m._artist;
	_album = m._artist;
	_title = m._title;
	_genre = m._genre;

	return *this;
}

QString MediaItem::displayText() {
  if (isNull())
    return QString::null;
  if (_artist == "Unknown")
    return _title;
  return _artist + " - " + _title;
};
