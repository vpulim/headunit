#include "HeadUnit.h"
#include "AudioPlayerScreen.h"
#include "XineVisPanel.h"
#include "Button.h"

XineVisPanel::XineVisPanel(QWidget *parent) 
  : XinePanel(parent)
{
  connect( buttons[PLAY], SIGNAL(clicked()), audioPlayer, SLOT(play()) );
  connect( buttons[STOP], SIGNAL(clicked()), audioPlayer, SLOT(stop()) );
  connect( buttons[PREV], SIGNAL(clicked()), audioPlayer, SLOT(previous()) );
  connect( buttons[NEXT], SIGNAL(clicked()), audioPlayer, SLOT(next()) );
  connect( buttons[EXIT], SIGNAL(clicked()), audioPlayer, SLOT(show()) );
  connect( buttons[EXIT], SIGNAL(clicked()), parent, SLOT(hide()) );
}

