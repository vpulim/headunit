#include <qapplication.h>
#include <qframe.h>
#include <qfileinfo.h>
#include <xine.h>
#include <xine/xineutils.h>
#include "XineVisPanel.h"
#include "XineVideoPanel.h"
#include "XineDVDPanel.h"
#include "XineVideo.h"
#include "HeadUnit.h"
#include "MenuScreen.h"

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

XineVideo::XineVideo() : FunctionScreen("XineVideo")
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

  visPanel = new XineVisPanel(this);
  if (visPanel->isNull()) return;
  videoPanel = new XineVideoPanel(this);
  if (videoPanel->isNull()) return;
  dvdPanel = new XineDVDPanel(this);
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
  volume = 100;
  volumeUp();
  mute = false;

  openedMRL = QString::null;
  playing = false;
  panel_on = false;
  
  valid = true;
}

void XineVideo::init() 
{
  connect( qApp, SIGNAL(aboutToQuit()), this, SLOT(cleanup()) );
}

void XineVideo::redirectOutput(QWidget *w)
{
  xine_port_send_gui_data(vo_port, XINE_GUI_SEND_DRAWABLE_CHANGED, 
			  (void*)(w->winId()));
}

void XineVideo::restoreOutput()
{
  xine_port_send_gui_data(vo_port, XINE_GUI_SEND_DRAWABLE_CHANGED, 
			  (void*)(winId()));
}

void XineVideo::cleanup() 
{
  close();
  //  xine_event_dispose_queue(event_queue);
  xine_dispose(audio_stream);
  xine_dispose(av_stream);
  xine_close_audio_driver(xine, ao_port);  
  xine_close_video_driver(xine, vo_port);  
  xine_exit(xine);
}

void XineVideo::endStream()
{
  playing = false;
}

const char *XineVideo::getArtist()
{
  if (openedMRL.isNull()) return NULL;
  return xine_get_meta_info(stream, XINE_META_INFO_ARTIST);  
}

const char *XineVideo::getAlbum()
{
  if (openedMRL.isNull()) return NULL;
  return xine_get_meta_info(stream, XINE_META_INFO_ALBUM);  
}

const char *XineVideo::getTitle()
{
  if (openedMRL.isNull()) return NULL;
  const char *title = xine_get_meta_info(stream, XINE_META_INFO_TITLE);  
  if (title)
    return title;

  if (openedMRL.startsWith("file://")) {
    QFileInfo fi(openedMRL.mid(7));
    strcpy(buf, fi.fileName().latin1());
    return buf;;
  }
  else if (openedMRL.startsWith("dvd://")) {
    return "DVD";
  }
  else
    return "Unknown";  
}

const char *XineVideo::getGenre()
{
  if (openedMRL.isNull()) return NULL;
  return xine_get_meta_info(stream, XINE_META_INFO_GENRE);  
}

bool XineVideo::getPosition(int *pos, int *len)
{
  int pos_stream;
  if (openedMRL.isNull()) {
    *pos = 0;
    *len = 0;
    return false;
  }
  return xine_get_pos_length (stream, &pos_stream, pos, len) == 1;
}

void XineVideo::open(const QString &mrl) 
{
  close();
  if (!xine_open(stream, mrl.latin1())) {
    qWarning("Unable to open location: %s", mrl.latin1());
    return;
  }
  if (xine_get_stream_info(stream, XINE_STREAM_INFO_HAS_VIDEO)) {
    if (stream != av_stream) {
      close();
      stream = av_stream;
      xine_open(stream, mrl.latin1()); 
    }
  }
  else {
    if (stream != audio_stream) {
      close();
      stream = audio_stream;
      xine_open(stream, mrl.latin1()); 
    }
  }
  openedMRL = mrl;
  playing = false;
}

void XineVideo::close() 
{
  if (!openedMRL.isNull())
    xine_close(stream);
  openedMRL = QString::null;
  playing = false;
}

void XineVideo::play() 
{
  if (openedMRL.isNull() || playing)
    return;
   if (!xine_play(stream, 0, 0)) {
    qWarning("Unable to play");
    return;
  }  
  playing = true;
}

void XineVideo::stop() 
{
  playing = false;
  if (openedMRL.isNull())
    return;
  xine_stop(stream);
}

void XineVideo::volumeUp()
{
  volume += 5;
  if (volume > 100)
    volume = 100;
  xine_set_param (audio_stream, XINE_PARAM_AUDIO_VOLUME, volume);  
  xine_set_param (av_stream, XINE_PARAM_AUDIO_VOLUME, volume);  
}

void XineVideo::volumeDown()
{
  volume -= 5;
  if (volume < 0)
    volume = 0;
  xine_set_param (audio_stream, XINE_PARAM_AUDIO_VOLUME, volume);  
  xine_set_param (av_stream, XINE_PARAM_AUDIO_VOLUME, volume);  
}

void XineVideo::volumeMute()
{
  if (!mute) {
    xine_set_param (audio_stream, XINE_PARAM_AUDIO_VOLUME, 0);  
    xine_set_param (av_stream, XINE_PARAM_AUDIO_VOLUME, 0);  
    mute = true;
  }
  else {
    xine_set_param (audio_stream, XINE_PARAM_AUDIO_VOLUME, volume);  
    xine_set_param (av_stream, XINE_PARAM_AUDIO_VOLUME, volume);  
    mute = false;
  }
}

void XineVideo::showAsVis()
{
  if (panel_on) activePanel->hide();    
  activePanel = visPanel;
  if (panel_on) activePanel->show();    
  show();
}

void XineVideo::showAsDVD() 
{
  if (panel_on) activePanel->hide();    
  activePanel = dvdPanel;
  if (panel_on) activePanel->show();    
  close();
  open("dvd://");
  play();
  show();
}

void XineVideo::showAsVideo() 
{
  if (panel_on) activePanel->hide();    
  activePanel = videoPanel;
  if (panel_on) activePanel->show();    
  show();
}

void XineVideo::moveEvent ( QMoveEvent *e ) 
{
  xpos = e->pos().x();
  ypos = e->pos().y();
}

void XineVideo::resizeEvent ( QResizeEvent *e ) 
{
  xw = e->size().width();
  xh = e->size().height();
  visPanel->resize(e->size().width(), 200);
  visPanel->move(0,e->size().height() - visPanel->size().height());
  dvdPanel->resize(e->size().width(), 200);
  dvdPanel->move(0,e->size().height() - dvdPanel->size().height());
  videoPanel->resize(e->size().width(), 200);
  videoPanel->move(0,e->size().height() - videoPanel->size().height());
}

void XineVideo::mouseReleaseEvent ( QMouseEvent * e )
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

void XineVideo::keyPressEvent ( QKeyEvent * e )
{
  if (e->key() == Qt::Key_Escape) {
    hide();
    menu->show();
  }
}
