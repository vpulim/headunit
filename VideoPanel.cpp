#include "HeadUnit.h"
#include "AudioPlayerScreen.h"
#include "MenuScreen.h"
#include "VideoPanel.h"
#include "Button.h"

VideoPanel::VideoPanel(QWidget *parent) 
  : Panel(parent)
{
  connect( buttons[PLAY], SIGNAL(clicked()), audioPlayer, SLOT(play()) );
  connect( buttons[STOP], SIGNAL(clicked()), audioPlayer, SLOT(stop()) );
  connect( buttons[PREV], SIGNAL(clicked()), audioPlayer, SLOT(previous()) );
  connect( buttons[NEXT], SIGNAL(clicked()), audioPlayer, SLOT(next()) );
  connect( buttons[EXIT], SIGNAL(clicked()), parent, SLOT(lower()) );
}

