#ifndef FILE_ITEM_H
#define FILE_ITEM_H

#include <qlistbox.h>

class FileItem : public QListBoxText
{
 public:
  FileItem(const QString &file, QListBox *listbox = 0);
  QString text() const;  
  QString fileName() { return file; };

 private:
  QString file;
};

#endif
