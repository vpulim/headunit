#include <qapplication.h>
#include <qframe.h>
#include <qfileinfo.h>
#include <qsettings.h>
#include <windows.h>
#include <DShow.h>
#include "HeadUnit.h"
#include "VisPanel.h"
#include "VideoPanel.h"
#include "DVDPanel.h"
#include "MediaPlayer.h"
#include "MenuScreen.h"
#include "MediaItem.h"
#include "ApplicationState.h"

IGraphBuilder*	graphBuilder;
IMediaControl*	mediaControl;
IMediaEventEx*	mediaEvent;
IMediaSeeking*	mediaSeek;
IVideoWindow*   videoWindow;
IBasicAudio*   basicAudio;

#define DS_RELEASE(x) if(x!=NULL){x->Release();x=NULL;}

MediaPlayer::MediaPlayer() : FunctionScreen("MediaPlayer")
{
  setPaletteBackgroundColor(QColor(0,0,0));
  resize(menu->size().width(), menu->size().height());

  // init DirectShow
  mediaSeek = NULL;
  mediaEvent = NULL;
  mediaControl = NULL;
  graphBuilder = NULL;
  videoWindow = NULL;
  basicAudio = NULL;

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
}

void MediaPlayer::cleanup() 
{
  closeItem();
}

bool MediaPlayer::getPosition(long *pos, long *len)
{
  if (openedItem.isNull() || !mediaSeek) {
    *pos = 0;
    *len = 0;
    return false;
  }
  LONGLONG llpos;
  LONGLONG lllen;
  mediaSeek->GetPositions(&llpos, &lllen);
  *pos = (long)(llpos / 10000);
  *len = (long)(lllen / 10000);
  return true;
}

bool MediaPlayer::setPosition(long pos)
{
  if (openedItem.isNull() || !mediaSeek) {
    return false;
  }
  LONGLONG llpos = pos * 10000;
  mediaSeek->SetPositions(&llpos, AM_SEEKING_AbsolutePositioning,
                          NULL, AM_SEEKING_NoPositioning);
  return true;
}

void MediaPlayer::openItem(const MediaItem &m) 
{
  closeItem();
  QString mrl = m.mrl();
  if (mrl.startsWith("file://"))
    mrl = mrl.mid(7);

  if (FAILED(CoCreateInstance(CLSID_FilterGraph, 
	  NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder,
	  (void**)&graphBuilder)))
    return;

  mrl.replace(QChar('/'), "\\\\");
  WCHAR wfilename[1024];
  mbstowcs(wfilename, mrl.latin1(), 1024);
  if (FAILED(graphBuilder->RenderFile(wfilename, NULL)))
    qWarning("open failed: %s", mrl.latin1());
  else {
	long lVisible;
    graphBuilder->QueryInterface(IID_IMediaControl, (void**)&mediaControl);
    graphBuilder->QueryInterface(IID_IMediaEvent, (void**)&mediaEvent);
    graphBuilder->QueryInterface(IID_IMediaSeeking, (void**)&mediaSeek);
    graphBuilder->QueryInterface(IID_IVideoWindow, (void**)&videoWindow);
    graphBuilder->QueryInterface(IID_IBasicAudio, (void**)&basicAudio);
	if (videoWindow && !FAILED(videoWindow->get_Visible(&lVisible))) {
	  videoWindow->put_Owner((OAHWND)winId());
	  videoWindow->put_MessageDrain((OAHWND)winId());
      videoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	  videoWindow->SetWindowPosition(0,0,size().width(), size().height());
	}
	if (basicAudio) {
	  basicAudio->put_Volume(volume*100 - 10000);
	}
	openedItem = m;
  }
}

#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

void MediaPlayer::closeItem() 
{
  stop();
  openedItem = MediaItem::null;
  if (videoWindow) {
    videoWindow->put_Visible(OAFALSE);
    videoWindow->put_Owner(NULL);
  }
  DS_RELEASE(mediaSeek);
  DS_RELEASE(mediaEvent);
  DS_RELEASE(mediaControl);
  DS_RELEASE(graphBuilder);
  DS_RELEASE(videoWindow);
  DS_RELEASE(basicAudio);
}

void MediaPlayer::play() 
{
  if (openedItem.isNull() || playState == PLAYING)
    return;
  mediaControl->Run();
  playState = PLAYING;
}

void MediaPlayer::pause() 
{
  if (openedItem.isNull() || playState != PLAYING)
    return;
  mediaControl->Pause();
  playState = PAUSED;
}

void MediaPlayer::stop() 
{
  playState = STOPPED;
  if (openedItem.isNull())
    return;
  mediaControl->Stop();
}

void MediaPlayer::setVolume(int volume)
{ 
  if (volume > 100)
    volume = 100;
  else if (volume < 0)
    volume = 0;
  appState->volume = this->volume = volume;
  if (basicAudio)
    basicAudio->put_Volume(volume*100 - 10000);
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
  if (panel_on) activePanel->hide();    
  activePanel = visPanel;
  if (panel_on) activePanel->show();    
  display();
}

void MediaPlayer::showAsDVD() 
{
  if (panel_on) activePanel->hide();    
  activePanel = dvdPanel;
  if (panel_on) activePanel->show();    
  closeItem();

  IDvdGraphBuilder *pDvdGB = NULL;
  CoCreateInstance(CLSID_DvdGraphBuilder, NULL, 
	  CLSCTX_INPROC_SERVER, IID_IDvdGraphBuilder,
	  reinterpret_cast<void**>(&pDvdGB));
  pDvdGB->GetFiltergraph(&graphBuilder);
  AM_DVD_RENDERSTATUS buildStatus={0};
  if (FAILED(pDvdGB->RenderDvdVideoVolume(NULL, AM_DVD_HWDEC_PREFER, &buildStatus)))
    qWarning("couldn't open DVD");
  else {
	long lVisible;
    graphBuilder->QueryInterface(IID_IMediaControl, (void**)&mediaControl);
    graphBuilder->QueryInterface(IID_IMediaEvent, (void**)&mediaEvent);
    graphBuilder->QueryInterface(IID_IMediaSeeking, (void**)&mediaSeek);
    graphBuilder->QueryInterface(IID_IVideoWindow, (void**)&videoWindow);
    graphBuilder->QueryInterface(IID_IBasicAudio, (void**)&basicAudio);
	if (videoWindow && !FAILED(videoWindow->get_Visible(&lVisible))) {
	  videoWindow->put_Owner((OAHWND)winId());
	  videoWindow->put_MessageDrain((OAHWND)winId());
      videoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	  videoWindow->SetWindowPosition(0,0,size().width(), size().height());
	}
	if (basicAudio) {
	  basicAudio->put_Volume(volume*100 - 10000);
	}
	MediaItem mi(-1, "", "", "DVD", "", "dvd://");
	openedItem = mi;
  }
  play();
  display();
}

void MediaPlayer::showAsVideo() 
{
  if (panel_on) activePanel->hide();    
  activePanel = videoPanel;
  if (panel_on) activePanel->show();    
  display();
}

void MediaPlayer::moveEvent ( QMoveEvent *e ) 
{
//  xpos = e->pos().x();
//  ypos = e->pos().y();
}

void MediaPlayer::resizeEvent ( QResizeEvent *e ) 
{
//  xw = e->size().width();
//  xh = e->size().height();
  visPanel->move(0,e->size().height() - visPanel->size().height());
  dvdPanel->move(0,e->size().height() - dvdPanel->size().height());
  videoPanel->move(0,e->size().height() - videoPanel->size().height());
}

void MediaPlayer::mouseReleaseEvent ( QMouseEvent * e )
{
  if (panel_on) {
    activePanel->hide();
    panel_on = false;
  }
  else {
    activePanel->show();
	BringWindowToTop(activePanel->winId());
    panel_on = true;
  }
}

void MediaPlayer::keyPressEvent ( QKeyEvent * e )
{
  if (e->key() == Qt::Key_Escape) {
    lower();
    // menu->show();
  }
}
