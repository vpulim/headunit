#include "HeadUnit.h"
#include "MenuScreen.h"
#include "MediaPlayer.h"
#include "DVDPanel.h"
#include "Button.h"

DVDPanel::DVDPanel(QWidget *parent) 
  : Panel(parent)
{
  connect( buttons[PLAY], SIGNAL(clicked()), parent, SLOT(play()) );
  connect( buttons[STOP], SIGNAL(clicked()), parent, SLOT(stop()) );
  connect( buttons[EXIT], SIGNAL(clicked()), parent, SLOT(lower()) );
}

