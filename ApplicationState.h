#ifndef APPLICATION_STATE_H
#define APPLICATION_STATE_H

class ApplicationState
{
  public:
    QString musicPath;
    bool musicPlus;
    int musicIndex;
    long musicPos;
    QString videoPath;
    int videoIndex;
    long videoPos;
    int dvdTitle;
    int dvdChapter;
    long dvdPos;
    int playMode;
    int volume;
    int function;

    enum { NONE = 0, MUSIC, VIDEO, DVD, NUM_FUNCTIONS };
};

#endif
