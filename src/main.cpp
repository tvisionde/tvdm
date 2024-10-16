#include <time.h>
#include <linux/reboot.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>

#include "main.h"
#include "destination.h"

void TClockView::draw()
{
    TDrawBuffer buffer;
    TColorAttr c = getColor(2);

    buffer.moveChar(0, ' ', c, (short)size.x);
    buffer.moveStr(0, currTime, c);
    writeLine(0, 0, (short)size.x, 1, buffer);
}

void TClockView::update()
{
    time_t t = time(0);
    char *date = ctime(&t);

    date[19] = '\0';
    strcpy(currTime, &date[11]);        /* Extract time. */

    if( strcmp(lastTime, currTime) )
    {
        drawView();
        strcpy(lastTime, currTime);
    }
}

Application::Application()
    : TProgInit( &Application::initStatusLine,
                 &Application::initMenuBar,
                 &Application::initDeskTop )
{
    TRect r = getExtent();

    // creates the clock view
    r.a.x = r.b.x - 9;
    r.b.y = r.a.y + 1;

    clock = new TClockView(r);
    clock->growMode = gfGrowLoX | gfGrowHiX;
    insert(clock);
}

TMenuBar* Application::initMenuBar(TRect r)
{
    r.b.y = r.a.y + 1;

    TSubMenu& mainMenu = \
        *new TSubMenu("Power", 0, hcNoContext) +
            *new TMenuItem("Restart", cmRestart, kbNoKey, hcNoContext) +
            *new TMenuItem("Shutdown", cmShutdown, kbNoKey, hcNoContext) +
            *new TMenuItem("Suspend", cmSuspend, kbNoKey, hcNoContext);

    return new TMenuBar(r, mainMenu + populateXSessions() + populateWaylandSessions());
}

void Application::handleEvent(TEvent& evt)
{
    TApplication::handleEvent(evt);

    switch (evt.what)
    {
        case evCommand:
            switch (evt.message.command)
            {
                case cmRestart:
                    if (messageBox("Are you sure want to reboot your computer? You will lost all unsaved changes!",
                                   mfConfirmation | mfYesButton | mfNoButton) == cmYes)
                    {
                        syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
                                LINUX_REBOOT_CMD_RESTART, NULL);
                    }
                    break;

                case cmShutdown:
                    if (messageBox("Are you sure want to shutdown your computer? You will lost all unsaved changes!",
                                   mfConfirmation | mfYesButton | mfNoButton) == cmYes)
                    {
                        syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
                                LINUX_REBOOT_CMD_POWER_OFF, NULL);
                    }
                    break;

                case cmSuspend:
                    syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
                            LINUX_REBOOT_CMD_SW_SUSPEND, NULL);
                    break;
            }
            break;
    }
    clearEvent(evt);
}

void Application::idle()
{
    TApplication::idle();
    clock->update();
}

int main(int argc, char** argv)
{
    Application *prog = new Application();
    prog->run();

    TObject::destroy(prog);
    return EXIT_SUCCESS;
}