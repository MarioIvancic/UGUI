#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

#include "x11-driver.h"

#include "ugui.h"

//Data Defines
typedef struct x11_data_s {
  Display *dis;
  Window win;
  GC gc;
  Visual *visual;
  int screen;
  uint32_t *imgBuffer;
  int simX;
  int simY;
} x11_data_t;

//Internal function declarations
void x11_pset(UG_S16 x, UG_S16 y, UG_COLOR c);

//Global Vars -- I hate these too
x11_data_t *handle;

bool x11_setup(int width, int height) {
  //Mem Alloc's
  handle = (x11_data_t *)malloc(sizeof(x11_data_t));
  if (NULL == handle) {
    return false;
  }
  handle->imgBuffer = (uint32_t *)malloc(width * height * sizeof(uint32_t));
  if (NULL == handle->imgBuffer) {
    return false;
  }
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
               ButtonReleaseMask|KeyPressMask);
  handle->gc = XCreateGC(handle->dis, handle->win, 0,0);
  XSetBackground(handle->dis, handle->gc, white);
  XSetForeground(handle->dis, handle->gc, black);
  XClearWindow(handle->dis, handle->win);
  XMapRaised(handle->dis, handle->win);
  handle->visual = DefaultVisual(handle->dis, 0);

  return true;
}

void *x11_getPsetFunctionPointer() {
  return (void *)&x11_pset;
}

//Process Function
void x11_process() {
  XEvent event;
  uint32_t *ximage = (uint32_t *)malloc(handle->simX *
                                       handle->simY *
                                       sizeof(uint32_t));

  //Check for events
  while (XCheckMaskEvent(handle->dis,
                         ButtonPressMask|ButtonReleaseMask,
                         &event) == true) {
    int x = event.xbutton.x;
    int y = event.xbutton.y;
    switch (event.type) {
      case ButtonPress:
        UG_TouchUpdate(x, y, TOUCH_STATE_PRESSED);
        break;

      case ButtonRelease:
        UG_TouchUpdate(-1, -1, TOUCH_STATE_RELEASED);

        break;
    }
  }

  if (ximage != NULL) {
    memcpy(ximage, handle->imgBuffer,
           handle->simX * handle->simY * sizeof(uint32_t));
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
void x11_pset(UG_S16 x, UG_S16 y, UG_COLOR c) {
  //Address 2d into 1d array
  handle->imgBuffer[handle->simX*y + x] = c;
}
