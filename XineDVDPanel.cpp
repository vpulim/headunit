#include "HeadUnit.h"
#include "MenuScreen.h"
#include "XineVideo.h"
#include "XineDVDPanel.h"

XineDVDPanel::XineDVDPanel(QWidget *parent) 
  : XinePanel(parent)
{
  connect( buttons[PLAY], SIGNAL(clicked()), parent, SLOT(play()) );
  connect( buttons[STOP], SIGNAL(clicked()), parent, SLOT(stop()) );
  connect( buttons[EXIT], SIGNAL(clicked()), menu, SLOT(show()) );
  connect( buttons[EXIT], SIGNAL(clicked()), parent, SLOT(hide()) );
}

