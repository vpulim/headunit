#include "HeadUnit.h"
#include "AudioPlayerScreen.h"
#include "MenuScreen.h"
#include "XineVideoPanel.h"

XineVideoPanel::XineVideoPanel(QWidget *parent) 
  : XinePanel(parent)
{
  connect( buttons[PLAY], SIGNAL(clicked()), audioPlayer, SLOT(play()) );
  connect( buttons[STOP], SIGNAL(clicked()), audioPlayer, SLOT(stop()) );
  connect( buttons[PREV], SIGNAL(clicked()), audioPlayer, SLOT(previous()) );
  connect( buttons[NEXT], SIGNAL(clicked()), audioPlayer, SLOT(next()) );
  connect( buttons[EXIT], SIGNAL(clicked()), menu, SLOT(show()) );
  connect( buttons[EXIT], SIGNAL(clicked()), parent, SLOT(hide()) );
}

