#include "HeadUnit.h"
#include "AudioPlayerScreen.h"
#include "VisPanel.h"
#include "Button.h"

VisPanel::VisPanel(QWidget *parent) 
  : Panel(parent)
{
  connect( buttons[PLAY], SIGNAL(clicked()), audioPlayer, SLOT(play()) );
  connect( buttons[STOP], SIGNAL(clicked()), audioPlayer, SLOT(stop()) );
  connect( buttons[PREV], SIGNAL(clicked()), audioPlayer, SLOT(previous()) );
  connect( buttons[NEXT], SIGNAL(clicked()), audioPlayer, SLOT(next()) );
  connect( buttons[EXIT], SIGNAL(clicked()), audioPlayer, SLOT(show()) );
  connect( buttons[EXIT], SIGNAL(clicked()), parent, SLOT(hide()) );
}

