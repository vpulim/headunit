#include "Tag.h"
#include <id3/misc_support.h>

Tag::Tag() : artist("Unknown"),
	     album("Unknown"),
	     title("Unknown"),
	     genre("Unknown")
{
}

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
}
