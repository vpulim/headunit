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

IGraphBuilder*	  graphBuilder;
IDvdGraphBuilder* dvdGraphBuilder;
IDvdControl2*     dvdControl;
IDvdInfo2*        dvdInfo;
IMediaControl*	  mediaControl;
IMediaEventEx*	  mediaEvent;
IMediaSeeking*	  mediaSeek;
IVideoWindow*     videoWindow;
IBasicAudio*      basicAudio;

#define DS_RELEASE(x) if(x!=NULL){x->Release();x=NULL;}

MediaPlayer::MediaPlayer(QWidget* parent) 
  : FunctionScreen("MediaPlayer", parent)
{
  setPaletteBackgroundColor(QColor(0,0,0));
  resize(menu->size().width(), menu->size().height());

  // init DirectShow
  mediaSeek = NULL;
  mediaEvent = NULL;
  mediaControl = NULL;
  graphBuilder = NULL;
  dvdGraphBuilder = NULL;
  dvdControl = NULL;
  dvdInfo = NULL;
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
  videoPanel->init();
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
  LONGLONG llpos = pos;
  llpos *= 10000;
  mediaSeek->SetPositions(&llpos, AM_SEEKING_AbsolutePositioning,
			  NULL, AM_SEEKING_NoPositioning);
  return true;
}

void MediaPlayer::openItem(const MediaItem &m) 
{
  closeItem();
  QString mrl = m.mrl();
  if (mrl.startsWith("file://")) {
    mrl = mrl.mid(7);
    if (FAILED(CoCreateInstance(CLSID_FilterGraph, 
				NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder,
				(void**)&graphBuilder))) {
      qWarning("Couldn't get the IGraphBuilder interface");
      return;
    }
  }
  else {
    qWarning("Unknown mrl type: %s",mrl.latin1());
    return;
  }
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
  DS_RELEASE(dvdGraphBuilder);
  DS_RELEASE(dvdControl);
  DS_RELEASE(dvdInfo);
  DS_RELEASE(videoWindow);
  DS_RELEASE(basicAudio);
}

void MediaPlayer::play() 
{
  if (playState == PLAYING && isDVD()) {
    int title, ch;
    long pos, len;
    DWORD dwFlags = DVD_CMD_FLAG_Block | DVD_CMD_FLAG_Flush;
    dvdGetLocation(&title,&ch,&pos,&len);
    if (title == 0)
      dvdControl->PlayTitle(1, dwFlags, NULL);    
  }
  if (openedItem.isNull() || playState == PLAYING)
    return;
  OAFilterState state;
  mediaControl->Run();
  mediaControl->GetState(5000, &state);
  if (state != State_Running) {
    stop();
    return;
  }
  playState = PLAYING;
  if (panel_on)
    activePanel->display();
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
  activePanel = visPanel;
  panel_on = true;
  display();
  visPanel->display();    
}

void MediaPlayer::showAsDVD() 
{
  activePanel = dvdPanel;
  panel_on = true;
  closeItem();

  if (FAILED(CoCreateInstance(CLSID_DvdGraphBuilder, NULL, 
			      CLSCTX_INPROC_SERVER, IID_IDvdGraphBuilder,
			      (void**)&dvdGraphBuilder))) {
    qWarning("Couldn't get the DvdGraphBuilder interface");
    return;
  }
  if (FAILED(dvdGraphBuilder->GetFiltergraph(&graphBuilder))) {
    qWarning("Couldn't get the GraphBuilder interface");
    closeItem();
    return;
  }
  AM_DVD_RENDERSTATUS buildStatus={0};
  if (FAILED(dvdGraphBuilder->RenderDvdVideoVolume(NULL, 
						   AM_DVD_HWDEC_PREFER, 
						   &buildStatus))) {
    qWarning("couldn't open DVD");
    closeItem();
    return;
  }
  if (FAILED(dvdGraphBuilder->GetDvdInterface(IID_IDvdControl2,
					      (void**)&dvdControl))) {
    qWarning("Couldn't get the IDvdControl2 interface");
    closeItem();
    return;
  }
  if (FAILED(dvdGraphBuilder->GetDvdInterface(IID_IDvdInfo2,
					      (void**)&dvdInfo))) {
    qWarning("Couldn't get the IDvdInfo2 interface");
    closeItem();
    return;
  }

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
  MediaItem dvd(-1,QString("dvd://"), QString(""),QString(""),QString("DVD"),QString(""));
  openedItem = dvd;
  play();
  display();
  dvdPanel->display();    
}

void MediaPlayer::showAsVideo() 
{
  activePanel = videoPanel;
  display();
  videoPanel->display();    
  panel_on = true;
}

void MediaPlayer::dvdPause() 
{
  if (openedItem.isNull() || !dvdControl || playState != PLAYING)
    return;
  dvdControl->Pause(TRUE);
  playState = PAUSED;
}

void MediaPlayer::dvdResume() 
{
  if (openedItem.isNull() || !dvdControl || playState != PAUSED)
    return;
  dvdControl->Pause(FALSE);
  playState = PLAYING;
}

bool MediaPlayer::dvdNextChapter()
{
  if (dvdControl) {
    DWORD dwFlags = DVD_CMD_FLAG_Block | DVD_CMD_FLAG_Flush;
    HRESULT result = dvdControl->PlayNextChapter(dwFlags, NULL);
    if (FAILED(result)) {
      int title, ch;
      long pos, len;
      dvdGetLocation(&title,&ch,&pos,&len);
      if (title == 0)
	dvdControl->PlayTitle(1, dwFlags, NULL);
    }
  }
  return false;
}

bool MediaPlayer::dvdPrevChapter()
{
  if (dvdControl) {
    DWORD dwFlags = DVD_CMD_FLAG_Block | DVD_CMD_FLAG_Flush;
    return dvdControl->PlayPrevChapter(dwFlags, NULL);
  }
  return false;
}

void MediaPlayer::dvdGetLocation(int *title, int *chapter, 
				 long *pos, long *len)
{
  *title = *chapter = *pos = *len = 0;
  if (dvdInfo) {
    DVD_PLAYBACK_LOCATION2 loc;
    DVD_HMSF_TIMECODE timeCode;
    if (FAILED(dvdInfo->GetCurrentLocation(&loc)))
      return;
    *title = (int)loc.TitleNum;
    *chapter = (int)loc.ChapterNum;
    long hour = loc.TimeCode.bHours;
    long min = loc.TimeCode.bMinutes;
    long sec = loc.TimeCode.bSeconds;
    *pos = (hour*3600+min*60+sec) * 1000;
    if (FAILED(dvdInfo->GetTotalTitleTime(&timeCode, NULL)))
      return;
    hour = timeCode.bHours;
    min = timeCode.bMinutes;
    sec = timeCode.bSeconds;    
    *len = (hour*3600+min*60+sec) * 1000;
  }
}

void MediaPlayer::moveEvent ( QMoveEvent *e ) 
{
  if (dvdControl) {
    POINT pt = { e->pos().x(), e->pos().y() };
    if (dvdControl->SelectAtPosition(pt) == S_OK)
      return;
  }
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
  if (dvdControl) {
    POINT pt = { e->pos().x(), e->pos().y() };
    if (dvdControl->ActivateAtPosition(pt) == S_OK)
      return;
  }
  if (panel_on) {
    activePanel->lower();
    panel_on = false;
  }
  else {
    activePanel->display();
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
