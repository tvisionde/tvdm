#pragma once

#define Uses_TMenuBar
#define Uses_TApplication
#define Uses_TMessageBox
#define Uses_TProgram
#define Uses_TMenuBar
#define Uses_TRect
#define Uses_TView
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TKeys
#define Uses_MsgBox

#include <tvision/tv.h>

enum commands
{
    cmRestart,
    cmShutdown,
    cmSuspend
};

class TClockView: public TView
{
private:
    char* lastTime = " ";
    char* currTime = " ";

public:
    TClockView(TRect& r): TView(r) {};
    virtual void draw();
    virtual void update();
};

class Application: public TApplication
{
private:
    TClockView* clock;

public:
    Application();

    static TMenuBar *initMenuBar(TRect r);
    virtual void handleEvent(TEvent& evt);
    virtual void idle();
};