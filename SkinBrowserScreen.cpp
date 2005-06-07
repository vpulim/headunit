#include <qapplication.h>
#include <qsettings.h>
#include <qdir.h>
#include <qlabel.h>
#include <qwmatrix.h>
#include "HeadUnit.h"
//#include "AudioPlayerScreen.h"
#include "SkinBrowserScreen.h"
#include "SelectionList.h"
#include "FileItem.h"
#include "Skin.h"
#include "DBHandler.h"

const char *SkinBrowserScreen::buttonKeys[SkinBrowserScreen::NUM_BUTTONS] = 
  { "SELECT", "EXIT", "PGDOWN", "DOWN", "UP", "PGUP" };

const char *SkinBrowserScreen::slKey = "S01";

SkinBrowserScreen::SkinBrowserScreen(QWidget* parent) 
  : FunctionScreen("SkinBrowser", parent)
{
   Skin skin("skin_browser.skin"); //MOVED to init() so that reload skin works
   if (skin.isNull()) {
     return;
   }
   skin.set(*this);
   for (int i=0; i<NUM_BUTTONS; i++) {
     buttons[i] = skin.getButton(buttonKeys[i], *this);
   }
  buttons[UP]->setAutoRepeat(true);
  buttons[DOWN]->setAutoRepeat(true);
  buttons[PGUP]->setAutoRepeat(true);
  buttons[PGDOWN]->setAutoRepeat(true);

  skinPreview = skin.getAlbumArt(*this);

   listView = skin.getSelectionList(slKey, *this);
  //rootDir = settings.readEntry( "headunit/musicpath" , "./" );
  //QDir rootDir("skins", QString::null, QDir::DirsFirst);
  //rootDir.setMatchAllDirs(true);
  //rootDir = rootDir.canonicalPath();
  valid = true;
}

void SkinBrowserScreen::init() 
{



  connect(buttons[EXIT], SIGNAL(clicked()), this, SLOT(lower()));
  connect(buttons[DOWN], SIGNAL(clicked()), listView, SLOT(scrollDown()));
  connect(buttons[UP], SIGNAL(clicked()), listView, SLOT(scrollUp()));
  connect(buttons[PGDOWN], SIGNAL(clicked()), listView,SLOT(scrollPageDown()));
  connect(buttons[PGUP], SIGNAL(clicked()), listView, SLOT(scrollPageUp()));
  connect(buttons[SELECT], SIGNAL(clicked()), this, SLOT(select()));
  //connect(buttons[PLUS], SIGNAL(clicked()), this, SLOT(selectFolderPlus()));
  //connect(buttons[BACK], SIGNAL(clicked()), this, SLOT(backFolder()));
  //connect(buttons[BROWSE], SIGNAL(clicked()), this, SLOT(browseFolder()));
  connect(listView,SIGNAL(highlighted(int)),this,SLOT(highlight(int)));
  connect(listView,SIGNAL(selected(int)),this, SLOT(select()));

  setDir();
}

void SkinBrowserScreen::setDir()
{
//   QDir dir(path, QString::null, QDir::Name | QDir::DirsFirst, QDir::Dirs);
//   if (!dir.isReadable())
//     return;
//   QString currPath = dir.canonicalPath();
//   if (!currPath.startsWith(rootDir))
//     return;

  listView->clear();


  QDir dir("skins", QString::null, QDir::DirsFirst);
  //dir.setMatchAllDirs(true);

  if (dir.isReadable()) { 	
	QFileInfoList *entries = (QFileInfoList *)dir.entryInfoList();
	QFileInfo *fi;
	for ( fi = entries->first(); fi; fi = entries->next() )
	    if (fi->fileName().at(0) != '.')
		listView->insertItem(fi->fileName());
    }
  //listBoxSkins->setSelected(0, TRUE);

  // read directories from file system
  //currDir = currPath;
  //QStringList entries = dir.entryList();
  //QStringList::ConstIterator it = entries.constBegin();
  //while (it != entries.constEnd()) {
//    QString entry = *it;
    //if (entry != "." && (entry != ".." || currDir != rootDir))
        //listView->insertDir(entry);
    //++it;
  //}
  
  //TODO displayAlbumArt(currDir);

  // read files from database using current directory as key
  //dbHandler->loadMediaList(currDir, false, musicList);
  //int size = musicList.size();
  //for (int i=0; i<size; i++) {
  //  listView->insertItem(musicList[i].displayText());
  //}
}

void SkinBrowserScreen::highlight(int index) 
{
  //TODO if (listView->isDir(index))
    //TODO displayAlbumArt(currDir + "/" + listView->dir(index));
    QString skinName("skins/");
    skinName.append(listView->text(index));
    skinName.append("/MENU_OFF.jpg");
    displaySkin(skinName);
	//QPixmap preview(skinName);
    //skinPreview->setPixmap(preview);
}

void SkinBrowserScreen::displaySkin(const QString& key) { //TODO
  QPixmap image(key);
  if (image.isNull()) {
    skinPreview->hide();
  }
  else {
    double scalew = skinPreview->width()/(double)image.width();
    double scaleh = skinPreview->height()/(double)image.height();
    QWMatrix m;
    m.scale(scalew,scaleh);
    QPixmap scaled = image.xForm(m);
    skinPreview->setPixmap(scaled);
    skinPreview->show();
  }
}

void SkinBrowserScreen::select() 
{

    //QSettings settings;
    //settings.setPath( "mp3car", "headunit" );
    QString skin = listView->currentText();
    if (!skin.isNull() && !skin.isEmpty()) {
	settings.writeEntry( "/headunit/skin", skin);
    }
    destroyGui();
    initializeGui();

//   if (listView->isDir(index))
//     setDir(currDir + "/" + listView->dir(index));
//   else {
//     emit folderSelected(currDir, false, 0, 0);
//     lower();
//   }
}
