#include "Tag.h"
#include <id3/misc_support.h>

Tag::Tag(QString file) : artist("Unknown"),
			 album("Unknown"),
			 title("Unknown"),
			 genre("Unknown")
{
  link(file.latin1());
}

void Tag::link(QString file)
{
  tag.Link(file);
  parse();
}

void Tag::parse() {
  char *str;

  if ((str = ID3_GetArtist(&tag)) != NULL)
    artist = str;

  if ((str = ID3_GetAlbum(&tag)) != NULL)
    album = str;

  if ((str = ID3_GetTitle(&tag)) != NULL)
    title = str;

  if ((str = ID3_GetGenre(&tag)) != NULL)
    genre = str;

  /*
  if (ID3_HasPicture(&tag)) {
    char picPath[1024] = "pic.tmp";

    ID3_Frame* frame = NULL;
    frame = tag.Find(ID3FID_PICTURE);
    if (frame != NULL)
    {
      ID3_Field* myField = frame->GetField(ID3FN_DATA);
      if (myField != NULL)
      {
	myField->ToFile(picPath);
	qWarning("writing file: %s (size=%d)",picPath, myField->Size());
      }
    }

    //    ID3_GetPictureData(&tag, picPath);
    //    picture.load(picPath);
    //    qWarning("loaded picture: %dx%d", picture.width(), picture.height());
  }
  */
}
