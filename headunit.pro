TEMPLATE	= app
LANGUAGE	= C++


INCLUDEPATH	+= .

HEADERS	+= AudioBrowserScreen.h \
	AudioPlayerScreen.h \
	Button.h \
	DVDPanel.h \
	FileItem.h \
	HeadUnit.h \
	MediaItem.h \
	MediaPlayer.h \
	MenuScreen.h \
	Panel.h \
	Screen.h \
	SelectionList.h \
	Skin.h \
	VideoPanel.h \
	VisPanel.h \
	DBHandler.h \
        MediaList.h \
        ApplicationState.h \
        Tag.h

SOURCES	+= AudioBrowserScreen.cpp \
	AudioPlayerScreen.cpp \
	Button.cpp \
	DVDPanel.cpp \
	FileItem.cpp \
	HeadUnit.cpp \
	MediaItem.cpp \
	MenuScreen.cpp \
	Panel.cpp \
	Screen.cpp \
	SelectionList.cpp \
	Skin.cpp \
	VideoPanel.cpp \
	VisPanel.cpp \
	DBHandler.cpp \
        Tag.cpp

FORMS	= ConfigDialog.ui

win32 {
  CONFIG  += console   # Comment out this line to disable console window
  SOURCES += DirectShowMediaPlayer.cpp
  INCLUDEPATH += d:/DX90SDK/Include
  LIBPATH += d:/DX90SDK/Lib
  LIBS += strmiids.lib
}

unix {
  SOURCES += XineMediaPlayer.cpp
  LIBS += `xine-config --libs` -lid3
}
