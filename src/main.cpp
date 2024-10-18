#include <time.h>
#include <sys/reboot.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <limits>

#define Uses_TWindow
#define Uses_TButton
#define Uses_TDialog
#define Uses_TStaticText
#define Uses_TInputLine

#include "main.h"
#include "pam.h"
#include "destination.h"

/* TPasswordInputLine */

void TPasswordInputLine::handleEvent(TEvent& event)
{
    // Call base class handler first
    TInputLine::handleEvent(event);

    // Only respond to key down events
    if (event.what == evKeyDown) {
        // Get ASCII value of the pressed key
        int charCode = event.keyDown.charScan.charCode;

        // Check if the character is printable
        if (charCode >= 32 && charCode <= 126) { // Printable ASCII range
            // Append the character to internal data (for storage)
            size_t length = strlen(data); // Get current length of data
            data[length] = static_cast<char>(charCode); // Store the actual character
            data[length + 1] = '\0'; // Null-terminate the string

            // Update display to show asterisk
            draw();
        } 
        // Handle backspace
        else if (event.keyDown.charScan.charCode == 27) {
            size_t length = strlen(data);
            if (length > 0) {
                data[length - 1] = '\0'; // Remove last character from internal data
                draw(); // Redraw to update display
            }
        }
    }
}

void TPasswordInputLine::draw()
{
    int l, r;
    TDrawBuffer b;

    TColorAttr color = getColor((state & sfFocused) ? 2 : 1);

    // Fill the buffer with spaces
    b.moveChar(0, ' ', color, size.x);

    if (size.x > 1 && data[0] != '\0') {
        // Fill with asterisks instead of actual data
        for (int i = 2; i <= displayedPos(curPos) - firstPos + 1; ++i) {
            b.moveChar(i, '*', color, 1); // Display '*' for each character
        }
    }

    // Draw scroll arrows if applicable
    if (canScroll(1))
        b.moveChar(size.x - 1, '\x10', getColor(4), 1);
    if (canScroll(-1))
        b.moveChar(0, '\x11', getColor(4), 1);

    // Highlight selected text if applicable
    if ((state & sfSelected) != 0) {
        l = displayedPos(selStart) - firstPos;
        r = displayedPos(selEnd) - firstPos;
        l = max(0, l);
        r = min(size.x - 2, r);
        if (l < r)
            b.moveChar(l + 1, 0, getColor(3), r - l); // Highlight selection
    }

    // Write the buffer to the screen
    writeLine(0, 0, size.x, size.y, b);

    // Set cursor position
    setCursor(displayedPos(curPos) - firstPos + 1, 0);
}

int TPasswordInputLine::displayedPos(int pos)
{
    return strwidth( TStringView(data, pos) );
}

Boolean TPasswordInputLine::canScroll(int delta)
{   
    return
        delta < 0
            ? Boolean(firstPos > 0)
            : delta > 0
                ? Boolean(strwidth(data) - firstPos + 2 > size.x)
                : False;
}

/* TClockView */

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
    strcpy(currTime, date);

    if(strcmp(lastTime, currTime))
    {
        drawView();
        strcpy(lastTime, currTime);
    }
}

/* Application */

Application::Application()
    : TProgInit( &Application::initStatusLine,
                 &Application::initMenuBar,
                 &Application::initDeskTop )
{
    TRect r = getExtent();

    // creates the clock view
    r.a.x = r.b.x - 20;
    r.b.y = r.a.y + 1;

    clock = new TClockView(r);
    clock->growMode = gfGrowLoX | gfGrowHiX;
    insert(clock);
    
    r = getExtent();
    
    auto t = new TDialog(
    TRect(
        (r.b.x ) / 2 - 55, // left
        (r.b.y - 20) / 2,      // top
        (r.b.x - 50) / 2 + 70,// right
        (r.b.y - 20) / 2 + 30   // bottom 
    ), "TurboVision DisplayManager");

    t->insert(
        new TStaticText(
            TRect(3, 2, 30, 3), "User name"
        )
    );

    userBox = new TInputLine(TRect(3, 4, 47, 5), std::numeric_limits<int>::max());
    t->insert(userBox);

    t->insert(
        new TStaticText(
            TRect(3, 6, 30, 10), "Password"
        )
    );

    passwordBox = new TPasswordInputLine(TRect(3, 8, 47, 9));
    t->insert(passwordBox);

    // a button width and height is not just the string length and 1...
    t->insert(
        new TButton(
            TRect(2, 10, 20, 11),
            "~L~ogin", cmLogin, bfDefault)
    );

    t->insert(
        new TButton(
            TRect(2 + 20, 10, 40, 11),
            "Leave a message", cmMessage, bfDefault
        )
    );

    execView(t);
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
                        reboot(RB_AUTOBOOT);
                    }
                    break;

                case cmShutdown:
                    if (messageBox("Are you sure want to shutdown your computer? You will lost all unsaved changes!",
                                   mfConfirmation | mfYesButton | mfNoButton) == cmYes)
                    {
                        reboot(RB_POWER_OFF);
                    }
                    break;

                case cmSuspend:
                    reboot(RB_SW_SUSPEND);
                    break;
                
                case cmLogin:
                    destroy(this);
                    break;
                
                case cmLogout:
                    logout();
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