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
#include <limits>

enum commands
{
    cmRestart,
    cmShutdown,
    cmSuspend,
    
    cmLogin,
    cmLogout,
    cmMessage
};

class TClockView: public TView
{
private:
    char lastTime[20];
    char currTime[20];

public:
    TClockView(TRect& r): TView(r)
    {
        strcpy(lastTime, "                    ");
        strcpy(currTime, "                    ");
    };
    virtual void draw();
    virtual void update();
};

class TPasswordInputLine: public TInputLine
{
public:
    TPasswordInputLine(const TRect& bounds)
        : TInputLine(bounds, std::numeric_limits<int>::max())
        {};
    
    virtual void handleEvent(TEvent& event) override;
    virtual void draw() override;

    // they are actually private functions, copied from original implementations

    int displayedPos(int pos);

    Boolean canScroll(int delta);
};

class Application: public TApplication
{
private:
    TClockView* clock;
    TPasswordInputLine* passwordBox;
    TInputLine* userBox;

public:
    Application();

    static TMenuBar *initMenuBar(TRect r);
    virtual void handleEvent(TEvent& evt);
    virtual void idle();
};