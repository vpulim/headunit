#include <qapplication.h>
#include <qsettings.h>
#include <qdir.h>
#include "HeadUnit.h"
#include "AudioPlayerScreen.h"
#include "AudioBrowserScreen.h"
#include "SelectionList.h"
#include "FileItem.h"
#include "Skin.h"

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

  audioFileFilter = "*";
  fileList = skin.getSelectionList(slKey, *this);
  rootDir = settings.readEntry( "headunit/musicpath" , "./" );
  QDir dir(rootDir);
  rootDir = dir.canonicalPath();
  valid = true;
}

void AudioBrowserScreen::init() 
{
  connect(buttons[EXIT], SIGNAL(clicked()), audioPlayer, SLOT(show()));
  connect(buttons[EXIT], SIGNAL(clicked()), this, SLOT(hide()));
  connect(buttons[DOWN], SIGNAL(clicked()), fileList, SLOT(scrollDown()));
  connect(buttons[UP], SIGNAL(clicked()), fileList, SLOT(scrollUp()));
  connect(buttons[PGDOWN], SIGNAL(clicked()), fileList,SLOT(scrollPageDown()));
  connect(buttons[PGUP], SIGNAL(clicked()), fileList, SLOT(scrollPageUp()));
  connect(buttons[SELECT], SIGNAL(clicked()), this, SLOT(selectHighlighted()));
  connect(buttons[PLUS], SIGNAL(clicked()), this, SLOT(selectHighlighted()));
  connect(fileList,SIGNAL(highlighted(int)),this,SLOT(highlight(int)));
  connect(fileList,SIGNAL(selected(int)),this, SLOT(select(int)));

  setDir(rootDir);
}

void AudioBrowserScreen::setDir(const QString &path)
{
  QDir dir(path, audioFileFilter, QDir::DirsFirst);
  dir.setMatchAllDirs(true);
  if (!dir.isReadable())
    return;
  QString currPath = dir.canonicalPath();
  if (!currPath.startsWith(rootDir))
    return;

  fileList->clear();
  
  QStringList entries = dir.entryList();
  QStringList::ConstIterator it = entries.constBegin();
  currDir = currPath;
  QString file = currDir + "/";
  while (it != entries.constEnd()) {
    if (*it != ".")
      fileList->insertItem(new FileItem(QString(file).append(*it)));
    ++it;
  }
}

void AudioBrowserScreen::highlight(int index) 
{
}

void AudioBrowserScreen::selectHighlighted() 
{
  select(fileList->currentItem());
}

void AudioBrowserScreen::select(int index) 
{
  QString path = ((FileItem *)(fileList->item(index)))->fileName();
  if (QFileInfo(path).isDir())
    setDir(path);
  else
    emit fileSelected(path);
}

