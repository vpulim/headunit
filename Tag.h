#ifndef TAG_H
#define TAG_H

#include <qstring.h>
#include <qpixmap.h>
#include <id3/tag.h>

class Tag
{
 public:
  Tag(QString file);
  void link(QString file);
  QString getArtist() { return artist; };
  QString getAlbum() { return album; };
  QString getTitle() { return title; };
  QString getGenre() { return genre; };
  QPixmap *getPicture() { return picture; };

 private:
  void parse();
  
  ID3_Tag tag;
  QString artist;
  QString album;
  QString title;
  QString genre;
  QPixmap *picture;
};

#endif
