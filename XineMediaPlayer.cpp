#include <qapplication.h>
#include <qframe.h>
#include <qfileinfo.h>
#include <qlayout.h>
#include "VisPanel.h"
#include "VideoPanel.h"
#include "DVDPanel.h"
#include "MediaPlayer.h"
#include "HeadUnit.h"
#include "MenuScreen.h"
#include "ApplicationState.h"
#include "qxinewidget.h"

QXineWidget *xine;

MediaPlayer::MediaPlayer(QWidget* parent) 
  : FunctionScreen("MediaPlayer", parent)
{
  setPaletteBackgroundColor(QColor(0,0,0));
  resize(menu->size().width(), menu->size().height());

  xine = new QXineWidget (this, "XineMediaPlayer", "auto", "xshm");
  xine->move(0,0);
  xine->resize(size().width(), size().height());
  xine->InitXine();

  visPanel = new VisPanel(this);
  if (visPanel->isNull()) return;
  videoPanel = new VideoPanel(this);
  if (videoPanel->isNull()) return;
  dvdPanel = new DVDPanel(this);
  if (dvdPanel->isNull()) return;
  activePanel = videoPanel;

  setVolume(appState->volume);
  mute = 0;

  openedItem = MediaItem::null;
  playState = STOPPED;
  panel_on = false;

  valid = true;
}

void MediaPlayer::init() 
{
  connect( qApp, SIGNAL(aboutToQuit()), this, SLOT(cleanup()) );
  videoPanel->init();
  setEraseColor(Qt::black);
}

void MediaPlayer::cleanup() 
{
  closeItem();
  delete xine;
}

bool MediaPlayer::getPosition(long *pos, long *len)
{
  return xine->GetPosition(pos, len);
}

bool MediaPlayer::setPosition(long pos)
{
  xine->slotChangePosition((int)pos);
}

void MediaPlayer::openItem(const MediaItem &mi) 
{
  closeItem();
  QString mrl = mi.mrl();
  if (!xine->PlayMRL(mrl.latin1(), "", FALSE)) {
    qWarning("Unable to open location: %s", mrl.latin1());
    return;
  }
  xine->SetVisualPlugin(QString("goom"));
  xine->slotSpeedPause();
  openedItem = mi;
}

void MediaPlayer::closeItem() 
{
  openedItem = MediaItem::null;
  playState = STOPPED;
}

void MediaPlayer::play() 
{
  if (openedItem.isNull() || playState == PLAYING)
    return;
  if (playState == STOPPED) 
    xine->slotStartPlayback();
  xine->slotSpeedNormal();
  playState = PLAYING;
}

void MediaPlayer::pause() 
{
  if (openedItem.isNull() || playState != PLAYING)
    return;
  xine->slotSpeedPause();
  playState = PAUSED;
}

void MediaPlayer::stop() 
{
  playState = STOPPED;
  if (openedItem.isNull())
    return;
  xine->slotStopPlayback();
}

void MediaPlayer::setVolume(int volume)
{ 
  if (volume > 100)
    volume = 100;
  else if (volume < 0)
    volume = 0;
  appState->volume = this->volume = volume;
  xine->slotSetVolume(-volume);
}

void MediaPlayer::volumeUp()
{
  if (mute) volumeMute();  // unmute if muted
  setVolume(volume + 5); 
}

void MediaPlayer::volumeDown()
{
  if (mute) volumeMute();  // unmute if muted
  setVolume(volume - 5); 
}

void MediaPlayer::volumeMute()
{
  if (!mute) {
    mute = volume;
    setVolume(0);
  }
  else {
    setVolume(mute);
    mute = 0;
  }
}

void MediaPlayer::showAsVis()
{
  activePanel = visPanel;
  panel_on = false;
  display();
  videoPanel->lower();    
  dvdPanel->lower();    
  visPanel->lower();    
}

void MediaPlayer::showAsDVD() 
{
  activePanel = dvdPanel;
  panel_on = false;
  if (!isDVD() && isOpened())
    closeItem();

  appState->function = ApplicationState::DVD;

  if (!(isDVD() && isPlaying())) {
    if (appState->dvdPos == -1) {
      MediaItem dvd(-1,QString("dvd://"), QString(""),QString(""),QString("DVD"),QString(""));
      openItem(dvd);
    }
    else {
      MediaItem dvd(-1,QString("dvd://%1.%2")
		    .arg(appState->dvdTitle).
		    arg(appState->dvdChapter), 
		    QString(""),QString(""),QString("DVD"),QString(""));
      openItem(dvd);
    }
    play();
  }
  display();
  visPanel->lower();
  videoPanel->lower();    
  dvdPanel->lower();    
}

void MediaPlayer::showAsVideo() 
{
  activePanel = videoPanel;
  panel_on = false;
  display();
  dvdPanel->lower();
  visPanel->lower();
  appState->function = ApplicationState::NONE;
  videoPanel->display();    
}

void MediaPlayer::dvdPause() 
{
  pause();
}

void MediaPlayer::dvdResume() 
{
  play();
}

bool MediaPlayer::dvdNextChapter()
{
  xine->PlayNextChapter();
  return true;
}

bool MediaPlayer::dvdPrevChapter()
{
  xine->PlayPreviousChapter();
  return true;
}

void MediaPlayer::dvdGetLocation(int *title, int *chapter, 
				 long *pos, long *len)
{
  xine->GetPosition(pos, len);
  *title = 0;
  *chapter = 0;
  QString titleString(xine->GetCurrentTitle());
  if (!titleString.startsWith("Title "))
    return;
  QString chapterString = titleString.section(", ", 1, 1);
  titleString = titleString.section(", ", 0, 0);
  *title = titleString.section(' ', 1, 1).toInt();
  *chapter = chapterString.section(' ', 1, 1).toInt();
}

void MediaPlayer::moveEvent ( QMoveEvent *e ) 
{
}

void MediaPlayer::resizeEvent ( QResizeEvent *e ) 
{
  visPanel->move(0,e->size().height() - visPanel->size().height());
  dvdPanel->move(0,e->size().height() - dvdPanel->size().height());
  videoPanel->move(0,e->size().height() - videoPanel->size().height());
}

void MediaPlayer::mouseMoveEvent ( QMouseEvent * e )
{
  //  FunctionScreen::mouseMoveEvent(e);
}

void MediaPlayer::mouseReleaseEvent ( QMouseEvent * e )
{
  if (panel_on) {
    activePanel->lower();
    panel_on = false;
    return;
  }
  if (isDVD() && xine->DVDButtonClicked) {
    return;
  }
  activePanel->display();
  panel_on = true;
}

void MediaPlayer::keyPressEvent ( QKeyEvent * e )
{
  if (e->key() == Qt::Key_Escape) {
    lower();
    // menu->show();
  }
}
