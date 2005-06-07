#include <qapplication.h>
#include <qdatetime.h>
#include <qtimer.h>
#include "HeadUnit.h"
#include "MenuScreen.h"
#include "AudioPlayerScreen.h"
#include "SkinBrowserScreen.h"
#include "VideoBrowserScreen.h"
#include "Skin.h"
#include "MediaPlayer.h"
#include "ConfigDialog.h"

const char *MenuScreen::keys[MenuScreen::NUM_BUTTONS] = 
  {"AUDIO","VIDEO","DVD","RADIO","LAUNCH","GPS","MUTE",
   "VOL-","VOL+","MIXER","APPEARANCE","EXIT","GAMMA"};

const char *MenuScreen::labelKeys[MenuScreen::NUM_LABELS] = 
  {"TIME","DATE"};

const char *MenuScreen::sliderKeys[MenuScreen::NUM_SLIDERS] = 
  {"MASTER"};


MenuScreen::MenuScreen(QWidget* parent) : FunctionScreen("Menu", parent)
{
  Skin skin("menu.skin");
  if (skin.isNull()) {
    return;
  }
  skin.set(*this);
  for (int i=0; i<NUM_BUTTONS; i++) {
    buttons[i] = skin.getButton(keys[i], *this);
  }
//  buttons[MUTE]->setToggleButton(true);

  for (int i=0; i<NUM_LABELS; i++) {
    labels[i] = skin.getLabel(labelKeys[i], *this);
  }

  for (int i=0; i<NUM_SLIDERS; i++) {
    sliders[i] = skin.getSlider(sliderKeys[i], *this);
  }

  updateTimer = new QTimer(this);

  valid = true;
}

void MenuScreen::init() {
  connect( buttons[AUDIO], SIGNAL(clicked()), audioPlayer, SLOT(display()) );
//  connect( buttons[AUDIO], SIGNAL(clicked()), this, SLOT(hide()) );
  connect( buttons[VIDEO], SIGNAL(clicked()), mediaPlayer, SLOT(showAsVideo()) );
  connect( buttons[DVD], SIGNAL(clicked()), mediaPlayer, SLOT(showAsDVD()) );
//  connect( buttons[DVD], SIGNAL(clicked()), this, SLOT(hide()) );
  connect( buttons[VOLUP], SIGNAL(clicked()), mediaPlayer, SLOT(volumeUp()) );
  connect( buttons[VOLDN], SIGNAL(clicked()), mediaPlayer, SLOT(volumeDown()) );
  connect( buttons[MUTE], SIGNAL(clicked()), mediaPlayer, SLOT(volumeMute()) );
  connect( buttons[EXIT], SIGNAL(clicked()), qApp, SLOT(quit()) );
  //connect( buttons[APPEAR], SIGNAL(clicked()), configDialog, SLOT(exec()) );
  connect( buttons[APPEAR], SIGNAL(clicked()), skinBrowser, SLOT(display()) );
  connect( updateTimer, SIGNAL(timeout()), this, SLOT(updateInfo()) );
  updateTimer->changeInterval(500);
}

void MenuScreen::updateInfo()
{
  labels[TIME]->setText(QTime::currentTime().toString("hh:mm:ss"));
  labels[DATE]->setText(QDate::currentDate().toString("d/M/yyyy"));
//  labels[VOLUME]->setText(QString::number(mediaPlayer->getVolume()) + "%");
  sliders[MASTER]->setValue(mediaPlayer->getVolume()/5);
}
