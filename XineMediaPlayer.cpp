#include <qapplication.h>
#include <qframe.h>
#include <qfileinfo.h>
#include <xine.h>
#include <xine/xineutils.h>
#include "VisPanel.h"
#include "VideoPanel.h"
#include "DVDPanel.h"
#include "MediaPlayer.h"
#include "HeadUnit.h"
#include "MenuScreen.h"
#include "ApplicationState.h"

static xine_t *xine;
static x11_visual_t vis;
static int                  xpos, ypos, xw, xh;
static double               pixel_aspect;
static xine_stream_t       *audio_stream;
static xine_stream_t       *av_stream;
static xine_stream_t       *stream;
static xine_event_queue_t  *event_queue;
static xine_video_port_t   *vo_port;
static xine_audio_port_t   *ao_port;

static void dest_size_cb(void *data, int video_width, int video_height, double video_pixel_aspect,
                         int *dest_width, int *dest_height, double *dest_pixel_aspect)  {
  *dest_width        = xw;
  *dest_height       = xh;
  *dest_pixel_aspect = pixel_aspect;
}

static void frame_output_cb(void *data, int video_width, int video_height,
                            double video_pixel_aspect, int *dest_x, int *dest_y,
                            int *dest_width, int *dest_height, 
                            double *dest_pixel_aspect, int *win_x, int *win_y) {
  *dest_x            = 0;
  *dest_y            = 0;
  *win_x             = xpos;
  *win_y             = ypos;
  *dest_width        = xw;
  *dest_height       = xh;
  *dest_pixel_aspect = pixel_aspect;
}

static void event_listener(void *user_data, const xine_event_t *event) {
  /*
  switch(event->type) { 
  case XINE_EVENT_UI_PLAYBACK_FINISHED:
    if (xineVideo != NULL)
      QTimer::singleShot(0, xineVideo, SLOT(endStream()));
    break;
  }
  
  case XINE_EVENT_PROGRESS:
    {
      xine_progress_data_t *pevent = (xine_progress_data_t *) event->data;
      
      printf("%s [%d%%]\n", pevent->description, pevent->percent);
    }
    break;
  */
}

void redirectOutput(QWidget *w)
{
  xine_port_send_gui_data(vo_port, XINE_GUI_SEND_DRAWABLE_CHANGED, 
			  (void*)(w->winId()));
}

void restoreOutput(QWidget *w)
{
  xine_port_send_gui_data(vo_port, XINE_GUI_SEND_DRAWABLE_CHANGED, 
			  (void*)(w->winId()));
}

MediaPlayer::MediaPlayer() : FunctionScreen("MediaPlayer")
{
  setPaletteBackgroundColor(QColor(0,0,0));
  resize(menu->size().width(), menu->size().height());
  xpos = pos().x();
  ypos = pos().y();
  xw = size().width();
  xh = size().height();
  pixel_aspect = 1.0;
  char configfile[2048];
  xine = xine_new();
  sprintf(configfile, "%s%s", xine_get_homedir(), "/.xine/config");
  xine_config_load(xine, configfile);
  xine_init(xine);

  visPanel = new VisPanel(this);
  if (visPanel->isNull()) return;
  videoPanel = new VideoPanel(this);
  if (videoPanel->isNull()) return;
  dvdPanel = new DVDPanel(this);
  if (dvdPanel->isNull()) return;
  activePanel = videoPanel;

  vis.display           = x11Display();
  vis.screen            = x11Screen();
  vis.d                 = winId();
  vis.dest_size_cb      = dest_size_cb;
  vis.frame_output_cb   = frame_output_cb;
  vis.user_data         = NULL;
  vo_port = xine_open_video_driver(xine, "sdl", XINE_VISUAL_TYPE_X11, (void *)&vis);
  ao_port = xine_open_audio_driver(xine , "auto", NULL);
  audio_stream = xine_stream_new(xine, ao_port, vo_port);
  av_stream = xine_stream_new(xine, ao_port, vo_port);
  stream = audio_stream;
  xine_post_t *plugin = xine_post_init(xine, "goom", 0, &ao_port, &vo_port);
  if (plugin != NULL)
    xine_post_wire(xine_get_audio_source (audio_stream), (xine_post_in_t *) xine_post_input (plugin, (char *) "audio in"));
  //  event_queue = xine_event_new_queue(stream);
  //  xine_event_create_listener_thread(event_queue, event_listener, NULL);
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
  //  xine_event_dispose_queue(event_queue);
  xine_dispose(audio_stream);
  xine_dispose(av_stream);
  xine_close_audio_driver(xine, ao_port);  
  xine_close_video_driver(xine, vo_port);  
  xine_exit(xine);
}

bool MediaPlayer::getPosition(long *pos, long *len)
{
  int pos_stream;
  *pos = 0;
  *len = 0;
  if (openedItem.isNull()) {
    return false;
  }
  int p, l;
  if(!xine_get_pos_length (stream, &pos_stream, &p, &l)) {
    return false;
  }
  *pos = p;
  *len = l;
  return true;
}

bool MediaPlayer::setPosition(long pos)
{
  if (openedItem.isNull()) {
    return false;
  }
  return xine_play (stream, 0, pos);
}

void MediaPlayer::openItem(const MediaItem &mi) 
{
  closeItem();
  QString mrl = mi.mrl();
  if (!xine_open(stream, mrl.latin1())) {
    qWarning("Unable to open location: %s", mrl.latin1());
    return;
  }
  if (xine_get_stream_info(stream, XINE_STREAM_INFO_HAS_VIDEO)) {
    if (stream != av_stream) {
      closeItem();
      stream = av_stream;
      xine_open(stream, mrl.latin1()); 
    }
  }
  else {
    if (stream != audio_stream) {
      closeItem();
      stream = audio_stream;
      xine_open(stream, mrl.latin1()); 
    }
  }
  openedItem = mi;
}

void MediaPlayer::closeItem() 
{
  if (!openedItem.isNull())
    xine_close(stream);
  openedItem = MediaItem::null;
  playState = STOPPED;
}

void MediaPlayer::play() 
{
  if (openedItem.isNull() || playState == PLAYING)
    return;
  if (playState == PAUSED) {
    xine_set_param (stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
  }
  else if (!xine_play(stream, 0, 0)) {
    qWarning("Unable to play");
    return;
  }  
  playState = PLAYING;
}

void MediaPlayer::pause() 
{
  if (openedItem.isNull() || playState != PLAYING)
    return;
  xine_set_param (stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
  playState = PAUSED;
}

void MediaPlayer::stop() 
{
  playState = STOPPED;
  if (openedItem.isNull())
    return;
  xine_stop(stream);
}

void MediaPlayer::setVolume(int volume)
{ 
  if (volume > 100)
    volume = 100;
  else if (volume < 0)
    volume = 0;
  appState->volume = this->volume = volume;
  xine_set_param (audio_stream, XINE_PARAM_AUDIO_VOLUME, volume);  
  xine_set_param (av_stream, XINE_PARAM_AUDIO_VOLUME, volume);  
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
  MediaItem dvd(-1,"","","DVD","","dvd://");
  openItem(dvd);
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
  xpos = e->pos().x();
  ypos = e->pos().y();
}

void MediaPlayer::resizeEvent ( QResizeEvent *e ) 
{
  xw = e->size().width();
  xh = e->size().height();
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
