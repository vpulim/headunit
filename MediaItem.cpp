#include <qfileinfo.h>
#include "MediaItem.h"

MediaItem::MediaItem(const QString &mrl, 
		     const QString &artist,
		     const QString &album,
		     const QString &title,
		     const QString &genre,
		     QListBox *listbox)
  : QListBoxText(listbox)
{
  _mrl = mrl;
  _artist = artist;
  _album = album;
  _title = title;
  _genre = genre;
}

QString MediaItem::text() const
{
  return _title;
}

