#ifndef MEDIA_ITEM_H
#define MEDIA_ITEM_H

#include <qlistbox.h>

class MediaItem : public QListBoxText
{
 public:
  MediaItem();
  MediaItem(const QString &mrl, 
	    const QString &artist,
	    const QString &album,
	    const QString &title,
	    const QString &genre,
	    QListBox *listbox = 0);
  MediaItem &operator= (const MediaItem &m);
  QString text() const;  
  QString mrl() const { return _mrl; };
  QString artist() const { return _artist; };
  QString album() const { return _album; };
  QString title() const { return _title; };
  QString genre() const { return _genre; };
  bool isNull() { return _mrl.isNull(); };

  static const MediaItem null;

 private:
  QString _mrl;
  QString _artist;
  QString _album;
  QString _title;
  QString _genre;
  QListBox *_listbox;
};

#endif
