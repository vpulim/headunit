#include <qdir.h>
#include <qfile.h>
#include <qimage.h>
#include <qsettings.h>
#include "HeadUnit.h"
#include "Button.h"
#include "SelectionList.h"
#include "Skin.h"

QString fixFileCase(QString path, QString file) {
  QDir dir(path);
  dir.setMatchAllDirs(true);

  QStringList entries = dir.entryList();
  QStringList::ConstIterator it = entries.constBegin();
  while (it != entries.constEnd()) {
    if ((*it).lower() == file.lower())
      return QString(*it);
    ++it;
  }
  return file;
}

Skin::Skin(const QString &skinFileName)
{
//  QString name = settings->readEntry("headunit/skin", "skin");
  QString name("skin");

  // load skin file for the specified type
  QString skin(name);
  skin.append(QChar('/'));
  skin.append(fixFileCase(name, skinFileName));
  QFile skinFile(skin);
  if (!skinFile.open( IO_ReadOnly )) {
    qWarning("couldn't load skin file: %s", skin.latin1());
    return;
  }

  // parse image file names
  char line[255] = " ";
  skinFile.readLine(line, 255);
  skinFile.readLine(line, 255);
  if (skinFile.readLine(line, 255) < 0 || line[0] == ' ' || line[1] == ',') {
    qWarning("invalid skin file: %s", skin.latin1());
    return;
  }
  QString files(line);
  files = files.stripWhiteSpace();
  
  QString empty(name);
  empty.append(QChar('/'));
  empty.append(fixFileCase(name, files.section(',',0,0)));
  QString off(name);
  off.append(QChar('/'));
  off.append(fixFileCase(name, files.section(',',1,1)));
  QString on(name);
  on.append(QChar('/'));
  on.append(fixFileCase(name, files.section(',',2,2)));
  QString down(name);
  down.append(QChar('/'));
  down.append(fixFileCase(name, files.section(',',3,3)));

  // load image files
  emptyImage = new QImage(empty);
  if (emptyImage->isNull()) {
    qWarning("couldn't load skin file: %s", empty.latin1());
    delete emptyImage;
    return;
  }  
  offImage = new QImage(off);
  if (offImage->isNull()) {
    qWarning("couldn't load skin file: %s", off.latin1());
    delete emptyImage; delete offImage;
    return;
  }  
  onImage = new QImage(on);
  if (onImage->isNull()) {
    qWarning("couldn't load skin file: %s", on.latin1());
    delete emptyImage; delete offImage; delete onImage;
    return;
  }  
  downImage = new QImage(down);
  if (downImage->isNull()) {
    qWarning("couldn't load skin file: %s", down.latin1());
    delete emptyImage; delete offImage; delete onImage; delete downImage;
    return;
  }  

  // parse skin file
  while (skinFile.readLine(line, 255) >= 0) {
    if (line[0] == ' ' || line[0] == '/' ||
	line[0] == '\r' || line[0] == '\n' ||
	(line[0] == 'E' && line[1] == 'N'))
      continue;
    QString l(line);
    l = l.stripWhiteSpace();
    QStringList values = QStringList::split(",", l, true);
    items += values;
  }
}

Skin::~Skin() 
{
  if (!isNull()) {
    delete emptyImage;
    delete offImage;
    delete onImage;
    delete downImage;
  }
}

void Skin::set(QFrame &f)
{
  f.setErasePixmap(QPixmap(*emptyImage));
  f.resize(emptyImage->width(), emptyImage->height());
}

Button *Skin::getButton(const char *c, QWidget &parent)
{
  QString code(c);
  code.prepend('"').append('"');
  SkinItemList::const_iterator it = items.constBegin();
  QStringList values;
  while ( it != items.constEnd()) {
    if ((*it).count() > 5 && (*it)[5] == code && (*it)[0][0] == 'B')
	  values = *it;
    ++it;
  }
  if (values.empty()) {
    qWarning("skin item %s is not defined!",code.latin1());
    return new Button(&parent, code);
  }
  Button *b = new Button(&parent, values[5]);
  int x = values[1].toInt();
  int y = values[2].toInt();
  int w = values[3].toInt();
  int h = values[4].toInt();
  b->move( x, y);
  b->resize( w, h);
  b->setOffPixmap(QPixmap(offImage->copy(x, y, w, h)));
  b->setOnPixmap(QPixmap(onImage->copy(x, y, w, h)));
  b->setDownPixmap(QPixmap(downImage->copy(x, y, w, h)));
  b->setFlat(true);
  return b;
}

QLabel *Skin::getLabel(const char *c, QWidget &parent)
{
  QString code(c);
  code.prepend('"').append('"');
  SkinItemList::const_iterator it = items.constBegin();
  QStringList values;
  while ( it != items.constEnd()) {
    if ((*it).count() > 10 && (*it)[10] == code && (*it)[0][0] == 'L')
	  values = *it;
    ++it;
  }
  if (values.empty()) {
    qWarning("skin item %s is not defined!",code.latin1());
    return new QLabel(&parent, code);
  }
  QLabel *l = new QLabel(&parent, values[10]);
  int x = values[1].toInt();
  int y = values[2].toInt();
  int w = values[3].toInt();
  int h = values[4].toInt();
  l->move( x, y);
  l->resize( w, h);
  l->setPaletteBackgroundPixmap(QPixmap(offImage->copy(x, y, w, h)));
  l->setFrameShape(QFrame::NoFrame);
  l->setFrameShadow(QFrame::Plain);
  QString fontDesc = values[9].remove('"');
  QString font = fontDesc.section(':',0,0);
  QString weight = fontDesc.section(':',1,1);
  bool bold = true;
  if (weight.isNull() || weight.isEmpty())
    bold = false;
  l->setPaletteForegroundColor(QColor(values[5].toInt(),
				      values[6].toInt(),
				      values[7].toInt()));
  l->setFont(QFont(font,(int)(values[8].toFloat() + 0.5), bold?QFont::Bold:QFont::Normal));
  //  l->setText(values[11]);
  return l;
}

SelectionList *Skin::getSelectionList(const char *key, QWidget &parent)
{
  SkinItemList::const_iterator it = items.constBegin();
  QStringList values;
  while ( it != items.constEnd()) {
    if (strcmp((*it)[0].latin1(), key) == 0) {
	  values = *it;
    }
    ++it;
  }
  if (values.empty()) {
    qWarning("skin item %s is not defined!",key);
    return new SelectionList(&parent, key);
  }
  SelectionList *sl = new SelectionList(&parent, values[5].latin1());
  int x = values[1].toInt();
  int y = values[2].toInt();
  int w = values[3].toInt();
  int h = values[4].toInt();
  QColor foreColor(values[5].toInt(),values[6].toInt(),values[7].toInt());
  //QColor foreColor(255,255,255);
  sl->move( x, y);
  sl->resize( w, h);
  sl->setFrameShape(QFrame::NoFrame);
  sl->setFrameShadow(QFrame::Plain);
  sl->setPaletteForegroundColor(foreColor);
  sl->setPaletteBackgroundPixmap(QPixmap(offImage->copy(x, y, w, h)));
  int offset = 0;
  int size = (int)(values[21].toFloat() + 0.5);
  // hack for now
  if (size > 36) {
	  offset = 3;
	  size = (int)(values[24].toFloat() + 0.5);
  }
  QString fontDesc = values[22+offset].remove('"');
  QString font = fontDesc.section(':',0,0);
  QString weight = fontDesc.section(':',1,1);
  bool bold = true;
  if (weight.isNull() || weight.isEmpty())
    bold = false;
  sl->setFont(QFont(font,size, bold?QFont::Bold:QFont::Normal));
  QPalette p = sl->palette();  
  QColorGroup cg = p.active();
  cg.setColor(QColorGroup::Highlight, Qt::black);
  cg.setColor(QColorGroup::HighlightedText, Qt::white);
  p.setActive(cg);
  sl->setPalette(p);

  return sl;
}
