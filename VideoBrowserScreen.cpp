#include <qapplication.h>
#include <qsettings.h>
#include <qdir.h>
#include "HeadUnit.h"
#include "VideoBrowserScreen.h"
#include "Skin.h"

const char *VideoBrowserScreen::buttonKeys[VideoBrowserScreen::NUM_BUTTONS] = 
  { "SELECT", "BACK", "BROWSE", "EXIT", "PGDOWN", "DOWN", "UP", "PGUP" };

const char *VideoBrowserScreen::slKey = "S01";

VideoBrowserScreen::VideoBrowserScreen(QWidget* parent) 
  : AudioBrowserScreen(QString("VideoBrowser"), parent)
{
  Skin skin("video_browser.skin");
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
  rootDir = settings.readEntry( "headunit/videopath" , "./" );
  QDir dir(rootDir);
  rootDir = dir.canonicalPath();
  valid = true;
}

void VideoBrowserScreen::init() 
{
  connect(buttons[EXIT], SIGNAL(clicked()), this, SLOT(lower()));
  connect(buttons[DOWN], SIGNAL(clicked()), listView, SLOT(scrollDown()));
  connect(buttons[UP], SIGNAL(clicked()), listView, SLOT(scrollUp()));
  connect(buttons[PGDOWN], SIGNAL(clicked()), listView,SLOT(scrollPageDown()));
  connect(buttons[PGUP], SIGNAL(clicked()), listView, SLOT(scrollPageUp()));
  connect(buttons[SELECT], SIGNAL(clicked()), this, SLOT(selectFolder()));
  connect(buttons[BACK], SIGNAL(clicked()), this, SLOT(backFolder()));
  connect(buttons[BROWSE], SIGNAL(clicked()), this, SLOT(browseFolder()));
  connect(listView,SIGNAL(highlighted(int)),this,SLOT(highlight(int)));
  connect(listView,SIGNAL(selected(int)),this, SLOT(select(int)));

  setDir(rootDir);
}
