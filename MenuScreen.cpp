#include <qapplication.h>
#include "HeadUnit.h"
#include "MenuScreen.h"
#include "AudioPlayerScreen.h"
#include "Skin.h"
#include "XineVideo.h"

const char *MenuScreen::keys[MenuScreen::NUM_BUTTONS] = 
  {"B01","B02","B03","B04","B05","B06","B07",
   "B08","B09","B10","B11","B12","B13"};

MenuScreen::MenuScreen() : FunctionScreen("Menu")
{
  Skin skin("menu.skin");
  if (skin.isNull()) {
    return;
  }
  skin.set(*this);
  for (int i=0; i<NUM_BUTTONS; i++) {
    buttons[i] = skin.getButton(keys[i], *this);
  }
  buttons[MUTE]->setToggleButton(true);
  valid = true;
}

void MenuScreen::init() {
  connect( buttons[AUDIO], SIGNAL(clicked()), audioPlayer, SLOT(show()) );
  connect( buttons[AUDIO], SIGNAL(clicked()), this, SLOT(hide()) );
  connect( buttons[VIDEO], SIGNAL(clicked()), xineVideo, SLOT(showAsVideo()) );
  connect( buttons[VIDEO], SIGNAL(clicked()), this, SLOT(hide()) );
  connect( buttons[DVD], SIGNAL(clicked()), xineVideo, SLOT(showAsDVD()) );
  connect( buttons[DVD], SIGNAL(clicked()), this, SLOT(hide()) );
  connect( buttons[VOLUP], SIGNAL(clicked()), xineVideo, SLOT(volumeUp()) );
  connect( buttons[VOLDN], SIGNAL(clicked()), xineVideo, SLOT(volumeDown()) );
  connect( buttons[MUTE], SIGNAL(clicked()), xineVideo, SLOT(volumeMute()) );
  connect( buttons[EXIT], SIGNAL(clicked()), qApp, SLOT(quit()) );
}
