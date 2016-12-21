#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

#include "simulation.h"
#include "ugui.h"

#define WIDTH 480
#define HEIGHT 272


static int mouse_down;

//Internal function declarations
void x11_pset(UG_S16 x, UG_S16 y, UG_COLOR c);
bool x11_setup(int width, int height);
void x11_process();

int main (void)
{
    printf("Simulation Application\n");

    //Setup X
    if (true != x11_setup(WIDTH, HEIGHT))
    {
        printf("Error Initializing x11 driver\n");
        return 0;
    }
    //Setup UGUI
    GUI_Setup(&x11_pset, WIDTH, HEIGHT);

    while (true)
    {
        GUI_Process();
        x11_process();
        usleep(100000);
    }
    return 0;
}

//Data Defines
typedef struct x11_data_s
{
    Display *dis;
    Window win;
    GC gc;
    Visual *visual;
    int screen;
    uint32_t *imgBuffer;
    int simX;
    int simY;
} x11_data_t;



//Global Vars -- I hate these too
x11_data_t *handle;

bool x11_setup(int width, int height)
{
    //Mem Alloc's
    handle = (x11_data_t *)malloc(sizeof(x11_data_t));
    if (NULL == handle) return false;

    handle->imgBuffer = (uint32_t *)malloc(width * height * sizeof(uint32_t));
    if (NULL == handle->imgBuffer) return false;
    handle->simX = width;
    handle->simY = height;

    //Setup X
    unsigned long black,white;

    handle->dis = XOpenDisplay((char *)0);
    handle->screen = DefaultScreen(handle->dis);
    black = BlackPixel(handle->dis, handle->screen),
    white = WhitePixel(handle->dis, handle->screen);
    handle->win = XCreateSimpleWindow(handle->dis,
                                  DefaultRootWindow(handle->dis),
                                  0, 0, width, height, 5, black, white);
    XSetStandardProperties(handle->dis, handle->win, "uGUI Window",
                         "uGUI", None, NULL, 0, NULL);
    XSelectInput(handle->dis, handle->win,
               ExposureMask|ButtonPressMask|
               ButtonReleaseMask|KeyPressMask|Button1MotionMask);
    handle->gc = XCreateGC(handle->dis, handle->win, 0,0);
    XSetBackground(handle->dis, handle->gc, white);
    XSetForeground(handle->dis, handle->gc, black);
    XClearWindow(handle->dis, handle->win);
    XMapRaised(handle->dis, handle->win);
    handle->visual = DefaultVisual(handle->dis, 0);

    return true;
}

// http://www.mi.uni-koeln.de/c/mirror/www.cs.curtin.edu.au/units/cg252-502/notes/lect5h1.html

//Process Function
void x11_process(void)
{
    XEvent event;
    uint32_t *ximage = (uint32_t *)malloc(handle->simX * handle->simY * sizeof(uint32_t));

    //Check for events
    while (XCheckMaskEvent(handle->dis,
        ButtonPressMask | ButtonReleaseMask | Button1MotionMask | PointerMotionMask, &event) == true)
    {
        //printf("X11 Event type %d\n", event.type);
        switch (event.type)
        {
            case ButtonPress:
                // we are interested only in LEFT button (button 1)
                if(event.xbutton.button != 1) break;
                mouse_down = 1;
                UG_TouchUpdate(event.xbutton.x, event.xbutton.y, TOUCH_STATE_PRESSED);
            break;

            case ButtonRelease:
                // we are interested only in LEFT button (button 1)
                if(event.xbutton.button != 1) break;
                mouse_down = 0;
                // UG_TouchUpdate(event.xbutton.x, event.xbutton.y, TOUCH_STATE_RELEASED);
                UG_TouchUpdate(-1, -1, TOUCH_STATE_RELEASED);
            break;

            case MotionNotify:
                //printf("Motion x=%d, y=%d, state=%d\n", event.xmotion.x, event.xmotion.y, event.xmotion.state & Button1Mask);
                if(mouse_down && ( event.xmotion.state & Button1Mask ))
                {
                    UG_TouchUpdate(event.xmotion.x, event.xmotion.y, TOUCH_STATE_PRESSED);
                    //UG_TouchUpdate(event.xmotion.x, event.xmotion.y, TOUCH_STATE_MOVED);
                    //printf("Touch Motion x=%d, y=%d\n", event.xmotion.x, event.xmotion.y);
                }
            break;
        }
    }

    if (ximage != NULL)
    {
        memcpy(ximage, handle->imgBuffer, handle->simX * handle->simY * sizeof(uint32_t));
        XImage *img = XCreateImage(handle->dis,
                               handle->visual,
                               24,
                               ZPixmap,
                               0,
                               (char *)ximage,
                               handle->simX,
                               handle->simY,
                               32,
                               0);
        XPutImage(handle->dis,
              handle->win,
              handle->gc,
              img,
              0,
              0,
              0,
              0,
              handle->simX,
              handle->simY);
        XDestroyImage(img);
        XFlush(handle->dis);
    }
}

//Internal
void x11_pset(UG_S16 x, UG_S16 y, UG_COLOR c)
{
    // Address 2d into 1d array
    handle->imgBuffer[handle->simX*y + x] = c;
}


static const char* message_type[3] = {"NONE", "WINDOW", "OBJECT"};
static const char* event_type[4] = {"NONE", "TOUCH", "PRERENDER", "POSTRENDER"};

void decode_msg(UG_MESSAGE* msg)
{
    printf("%s %s for ID %d (SubId %d)\n",
        message_type[msg->type], event_type[msg->event], msg->id, msg->sub_id);
}
