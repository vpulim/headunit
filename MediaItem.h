#ifndef MEDIA_ITEM_H
#define MEDIA_ITEM_H

#include <qlistbox.h>

class MediaItem : public QListBoxText
{
 public:
  MediaItem(const QString &mrl, 
	    const QString &artist,
	    const QString &album,
	    const QString &title,
	    const QString &genre,
	    QListBox *listbox = 0);
  QString text() const;  
  QString mrl() { return _mrl; };
  QString artist() { return _artist; };
  QString album() { return _album; };
  QString title() { return _title; };
  QString genre() { return _genre; };
  
 private:
  QString _mrl;
  QString _artist;
  QString _album;
  QString _title;
  QString _genre;
};

#endif
