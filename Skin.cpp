#include <qdir.h>
#include <qfile.h>
#include <qimage.h>
#include <qregexp.h>
#include <qsettings.h>
#include "HeadUnit.h"
#include "Button.h"
#include "SelectionList.h"
#include "Skin.h"
#include "Slider.h"

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
  QString skin("skins/");
  //settings.setPath( "mp3car", "headunit" );
  skin.append(settings.readEntry("headunit/skin", "Default"));

  // load skin file for the specified type
  QString name(skin);
  name.append(QChar('/'));
  name.append(fixFileCase(name, skinFileName));
  QFile skinFile(name);
  if (!skinFile.open( IO_ReadOnly )) {
    qWarning("couldn't load skin file: %s", name.latin1());
    return;
  }

  // parse image file names
  char line[255] = " ";
  skinFile.readLine(line, 255);
  skinFile.readLine(line, 255);
  if (skinFile.readLine(line, 255) < 0 || line[0] == ' ' || line[1] == ',') {
    qWarning("invalid skin file: %s", name.latin1());
    return;
  }
  QString files(line);
  files = files.stripWhiteSpace();
  
  QString empty(skin);
  empty.append(QChar('/'));
  empty.append(fixFileCase(skin, files.section(',',0,0)));
  QString off(skin);
  off.append(QChar('/'));
  off.append(fixFileCase(skin, files.section(',',1,1)));
  QString on(skin);
  on.append(QChar('/'));
  on.append(fixFileCase(skin, files.section(',',2,2)));
  QString down(skin);
  down.append(QChar('/'));
  down.append(fixFileCase(skin, files.section(',',3,3)));

  // load image files
  /** 
   * 127 = nutral, 64 = darker, 192 = lighter
   * I don't know if this is good enought for gamma
   **/
  int gammaVal = 127;

  //emptyImage = new QImage(empty);
  emptyImage = gamma(gammaVal, QImage(empty));
  if (emptyImage->isNull()) {
    qWarning("couldn't load EMPTY skin file: %s", empty.latin1());
    delete emptyImage;
    return;
  }  
  //offImage = new QImage(off);
  offImage = gamma(gammaVal, QImage(off));
  if (offImage->isNull()) {
    qWarning("couldn't load OFF skin file: %s", off.latin1());
    /*delete emptyImage; */delete offImage;
    return;
  }  
  //onImage = new QImage(on);
  onImage = gamma(gammaVal, QImage(on));
  if (onImage->isNull()) {
    qWarning("couldn't load ON skin file: %s (using OFF instead)", on.latin1());
    onImage = new QImage(off);
  }  
  //downImage = new QImage(down);
  downImage = gamma(gammaVal, QImage(down));
  if (downImage->isNull()) {
    qWarning("couldn't load DOWN skin file: %s (using OFF instead)", down.latin1());
    downImage = new QImage(off);
  }  

  // parse skin file

  while (skinFile.readLine(line, 255) >= 0) {
    if (line[0] == ' ' || line[0] == '/' ||
	line[0] == '\r' || line[0] == '\n' ||
	(line[0] == 'E' && line[1] == 'N')) {
      items += "";
      continue;
    }
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
    qWarning("skin item %s (button) is not defined!",code.latin1());
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
    qWarning("skin item %s (selection list)is not defined!",key);
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

QLabel *Skin::getIndicator(const char *c, QWidget &parent)
{
  QString code(c);
  code.prepend('"').append('"');
  SkinItemList::const_iterator it = items.constBegin();
  QStringList values;
  while ( it != items.constEnd()) {
    if ((*it).count() > 5 && (*it)[5] == code && (*it)[0][0] == 'I')
	  values = *it;
    ++it;
  }
  if (values.empty()) {
    qWarning("skin item %s (indicator) is not defined!",code.latin1());
    return new QLabel(&parent);
  }
  QLabel *l = new QLabel(&parent, values[5]);
  int x = values[1].toInt();
  int y = values[2].toInt();
  int w = values[3].toInt();
  int h = values[4].toInt();
  l->move( x, y);
  l->resize( w, h);
  l->setPixmap(QPixmap(onImage->copy(x, y, w, h)));
  l->setFrameShape(QFrame::NoFrame);
  l->setFrameShadow(QFrame::Plain);
  return l;
}

QLabel *Skin::getAlbumArt(QWidget &parent)
{
  SkinItemList::const_iterator it = items.constBegin();
  QStringList values;
  while ( it != items.constEnd()) {
    if ((*it)[0][0] == 'J')
	  values = *it;
    ++it;
  }
  if (values.empty()) {
    qWarning("skin item (cover art) is not defined!");
    return new QLabel(&parent);
  }
  QLabel *l = new QLabel(&parent);
  int x = values[1].toInt();
  int y = values[2].toInt();
  int w = values[3].toInt();
  int h = values[4].toInt();
  l->move( x, y);
  l->resize( w, h);
  l->setPixmap(QPixmap(emptyImage->copy(x, y, w, h)));
  l->setFrameShape(QFrame::NoFrame);
  l->setFrameShadow(QFrame::Plain);
  return l;
}

/**
 * Get Slider
 **/
Slider *Skin::getSlider(const char *c, QWidget &parent)
{
  QString code(c);
  code.prepend('"').append('"');
  SkinItemList::const_iterator it = items.constBegin();
  QStringList values;
  while ( it != items.constEnd()) {
    if ((*it).count() > 13 && (*it)[13] == code && (*it)[0][0] == 'S')
	  values = *it;
    ++it;
  }
  if (values.empty()) {
    qWarning("skin item %s (slider) is not defined!",code.latin1());
    return new Slider(&parent, code, Qt::Horizontal, 0, 0, 0);
  }
  Qt::Orientation orientation;
  int height;

  if (values[9].compare("H") == 0 ) {
    height = values[8].toInt();
    orientation = Qt::Horizontal;
  } else {
    height = values[12].toInt();
    orientation = Qt::Vertical;
  }

  
  int width = values[7].toInt();

  Slider *s = new Slider(&parent, values[13], orientation, 20, height, width );
  int x = values[1].toInt();
  int y = values[2].toInt();
  int w = values[3].toInt();
  int h = values[4].toInt();
  QString barPixPath("skins/");
  barPixPath.append(settings.readEntry("headunit/skin", "Default"));
  barPixPath.append("/");
  int i = values[8].toInt();
  QString v = values[11];

  values[11] = values[11].replace(values[11].find( '\\' , 0 ), 1, "/"); // Finds and replaces the \ to a /
  barPixPath.append(fixFileCase(barPixPath, values[11].mid(1, values[11].length()-2)));

  s->move( x, y);
  s->resize( w, h);
  s->setBarPixmap(QPixmap(barPixPath));
  s->setOrientation(orientation);
  //b->setFlat(true);
  return s;
}

QImage *Skin::gamma(int c, QImage image)
{

        //QImage newImage(image);
    if ( image == NULL )
                return new QImage();
        int dep = image.depth();
        int hei = image.height();
        int wid = image.width();
        unsigned char* bits = image.bits();
        int numc = image.numColors();
        int pixels;
        unsigned int * data;

        if(c > 255)
         c = 255;
     if(c < -255)
         c =  -255;
         if ( dep > 8 )
         {
                pixels = wid * hei ;
                data = (unsigned int*) bits ;
         }
         else
         {
                 pixels = numc;
                 data = image.colorTable();
         }
     for(int i=0; i < pixels; ++i){
         int r,g,b;
                 r = qRed(data[i]);
         g = qGreen(data[i]);
         b = qBlue(data[i]);

                 QColor myCol;
                 myCol.setRgb(r,g,b);
                 if ( c < 128 )
                 myCol = myCol.dark(100+(128-c));
                 else
                 myCol = myCol.light(100+(c-128));

                 data[i] = qRgba(myCol.red(),myCol.green(),myCol.blue(), 
qAlpha(data[i]));
         }
         return new QImage(image);

}