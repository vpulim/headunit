#include <qfileinfo.h>
#include "MediaItem.h"

const MediaItem MediaItem::null;

MediaItem::MediaItem()
{
  _mrl = QString::null;
  _artist = QString::null;
  _album = QString::null;
  _title = QString::null;
  _genre = QString::null;
  _listbox = NULL;
}

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
  _listbox = listbox;
}

MediaItem &MediaItem::operator= (const MediaItem &m)
{
	_mrl = m._mrl;
	_artist = m._artist;
	_album = m._artist;
	_title = m._title;
	_genre = m._genre;
	_listbox = m._listbox;

	return *this;
}

QString MediaItem::text() const
{
  return _title;
}

