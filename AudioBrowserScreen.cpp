#include <qapplication.h>
#include <qsettings.h>
#include <qdir.h>
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

AudioBrowserScreen::AudioBrowserScreen() : FunctionScreen("AudioBrowser")
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

  listView = skin.getSelectionList(slKey, *this);
  rootDir = settings.readEntry( "headunit/musicpath" , "./" );
  QDir dir(rootDir);
  rootDir = dir.canonicalPath();
  valid = true;
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

  // read files from database using current directory as key
  dbHandler->loadMusicList(currDir, false, musicList);
  int size = musicList.size();
  for (int i=0; i<size; i++) {
    listView->insertItem(musicList[i].title());
  }
}

void AudioBrowserScreen::highlight(int index) 
{
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
  emit folderSelected(path, plus, 0);
  lower();
}

void AudioBrowserScreen::select(int index) 
{
  if (listView->isDir(index))
    setDir(currDir + "/" + listView->dir(index));
  else {
    emit folderSelected(currDir, false, 0);
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
