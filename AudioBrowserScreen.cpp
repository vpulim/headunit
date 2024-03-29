#include <qapplication.h>
#include <qsettings.h>
#include <qdir.h>
#include <qlabel.h>
#include <qwmatrix.h>
#include "HeadUnit.h"
#include "AudioPlayerScreen.h"
#include "AudioBrowserScreen.h"
#include "SelectionList.h"
#include "FileItem.h"
#include "Skin.h"
#include "DBHandler.h"

const char *AudioBrowserScreen::buttonKeys[AudioBrowserScreen::NUM_BUTTONS] = 
  { "SELECT", "SELECT+", "BACK", "BROWSE", "EXIT", "PGDOWN", "DOWN", "UP", "PGUP" };

const char *AudioBrowserScreen::slKey = "S01";

AudioBrowserScreen::AudioBrowserScreen(QWidget* parent) 
  : FunctionScreen("AudioBrowser", parent)
{
  Skin skin("audio_browser.skin");
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

  cover = skin.getAlbumArt(*this);

  listView = skin.getSelectionList(slKey, *this);
  rootDir = settings.readEntry( "headunit/musicpath" , "./" );
  QDir dir(rootDir);
  rootDir = dir.canonicalPath();
  valid = true;
}

/**
 * initSkin(), Reloads the skin after a skin change
 **/
void AudioBrowserScreen::initSkin(){
  Skin skin("audio_browser.skin");
  if (skin.isNull()) {
    return;
  }
  skin.set(*this);
  for (int i=0; i<NUM_BUTTONS; i++) {
    buttons[i]->close();
    buttons[i] = skin.getButton(buttonKeys[i], *this);
  }
  buttons[UP]->setAutoRepeat(true);
  buttons[DOWN]->setAutoRepeat(true);
  buttons[PGUP]->setAutoRepeat(true);
  buttons[PGDOWN]->setAutoRepeat(true);

  cover->close();
  cover = skin.getAlbumArt(*this);

  listView->close();
  listView = skin.getSelectionList(slKey, *this);
  rootDir = settings.readEntry( "headunit/musicpath" , "./" );
  QDir dir(rootDir);
  rootDir = dir.canonicalPath();
}

void AudioBrowserScreen::init() 
{
  connect(buttons[EXIT], SIGNAL(clicked()), this, SLOT(lower()));
  connect(buttons[DOWN], SIGNAL(clicked()), listView, SLOT(scrollDown()));
  connect(buttons[UP], SIGNAL(clicked()), listView, SLOT(scrollUp()));
  connect(buttons[PGDOWN], SIGNAL(clicked()), listView,SLOT(scrollPageDown()));
  connect(buttons[PGUP], SIGNAL(clicked()), listView, SLOT(scrollPageUp()));
  connect(buttons[SELECT], SIGNAL(clicked()), this, SLOT(selectFolder()));
  connect(buttons[PLUS], SIGNAL(clicked()), this, SLOT(selectFolderPlus()));
  connect(buttons[BACK], SIGNAL(clicked()), this, SLOT(backFolder()));
  connect(buttons[BROWSE], SIGNAL(clicked()), this, SLOT(browseFolder()));
  connect(listView,SIGNAL(highlighted(int)),this,SLOT(highlight(int)));
  connect(listView,SIGNAL(selected(int)),this, SLOT(select(int)));

  setDir(rootDir);
}

void AudioBrowserScreen::setDir(const QString &path)
{
  QDir dir(path, QString::null, QDir::Name | QDir::DirsFirst, QDir::Dirs);
  if (!dir.isReadable())
    return;
  QString currPath = dir.canonicalPath();
  if (!currPath.startsWith(rootDir))
    return;

  listView->clear();

  // read directories from file system
  currDir = currPath;
  QStringList entries = dir.entryList();
  QStringList::ConstIterator it = entries.constBegin();
  while (it != entries.constEnd()) {
    QString entry = *it;
    if (entry != "." && (entry != ".." || currDir != rootDir))
        listView->insertDir(entry);
    ++it;
  }
  
  displayAlbumArt(currDir);

  // read files from database using current directory as key
  dbHandler->loadMediaList(currDir, false, musicList);
  int size = musicList.size();
  for (int i=0; i<size; i++) {
    listView->insertItem(musicList[i].displayText());
  }
}

void AudioBrowserScreen::highlight(int index) 
{
  if (listView->isDir(index))
    displayAlbumArt(currDir + "/" + listView->dir(index));
}

void AudioBrowserScreen::displayAlbumArt(const QString& key) {
  QPixmap image = dbHandler->loadAlbumArt(key);
  if (image.isNull()) {
    cover->hide();
  }
  else {
    double scalew = cover->width()/(double)image.width();
    double scaleh = cover->height()/(double)image.height();
    QWMatrix m;
    m.scale(scalew,scaleh);
    QPixmap scaled = image.xForm(m);
    cover->setPixmap(scaled);
    cover->show();
  }
}

void AudioBrowserScreen::selectFolder() 
{
  folderSelect(false);
}

void AudioBrowserScreen::selectFolderPlus() 
{
  folderSelect(true);
}

void AudioBrowserScreen::folderSelect(bool plus)
{
  QString path = currDir;
  int i = listView->currentItem();
  if (listView->isDir(i)) {
    if (listView->dir(i) == "..") {
      backFolder();
      return;
    }
    path += "/" + listView->dir(i);
  }
  emit folderSelected(path, plus, 0, 0);
  lower();
}

void AudioBrowserScreen::select(int index) 
{
  if (listView->isDir(index))
    setDir(currDir + "/" + listView->dir(index));
  else {
    emit folderSelected(currDir, false, 0, 0);
    lower();
  }
}

void AudioBrowserScreen::backFolder() 
{
  if (currDir != rootDir)
    setDir(currDir + "/..");
}

void AudioBrowserScreen::browseFolder()
{
  int index = listView->currentItem();
  if (listView->isDir(index))
    setDir(currDir + "/" + listView->dir(index));
}
