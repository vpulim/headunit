#include <qfileinfo.h>
#include "FileItem.h"

FileItem::FileItem(const QString &file, QListBox *listbox)
  : QListBoxText(listbox)
{
  this->file = file;
}

QString FileItem::text() const
{
  QFileInfo fi(file);
  if (fi.isDir())
    return fi.fileName().prepend('[').append(']');
  return fi.fileName();
}
