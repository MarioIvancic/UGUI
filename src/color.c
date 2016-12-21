#include <stdlib.h>

#include "ugui.h"
#include "simulation.h"

//Global Vars
#define MAX_OBJS 5
UG_GUI gui;
UG_WINDOW wnd;
UG_IMAGE img;
UG_OBJECT objs[MAX_OBJS];
UG_BMP bmp;


// Internal Functions
void windowHandler(UG_MESSAGE *msg);
void createHSVBMP(UG_BMP *bmp, int w, int h);


void GUI_Setup(void *pset, int w, int h){
  //Setup UGUI
  UG_Init(&gui, pset, w, h);

  //Setup Window
  UG_WindowCreate(&wnd, objs, MAX_OBJS, &windowHandler);
  UG_WindowSetTitleHeight(&wnd, 0);

  //Create HSV Chooser Image
  createHSVBMP(&bmp, 252, 255);

  UG_ImageCreate(&wnd, &img, IMG_ID_0, 112, 0, 252, 255);

  UG_ImageSetBMP(&wnd, IMG_ID_0, &bmp);

  //  UG_ImageShow(&wnd, IMG_ID_0);
  UG_WindowShow(&wnd);
}

void GUI_Process() {
  //
  UG_Update();
}

void windowHandler(UG_MESSAGE *msg) {
  (void)msg;
}

#define HSV_SECTION 42
// ref https://www.cs.rit.edu/~ncs/color/t_convert.html

uint32_t hsvToRgb(uint8_t h, uint8_t s, uint8_t v) {
  uint8_t r, g, b;
  uint8_t sect = h / HSV_SECTION;
  uint8_t subSection = h % HSV_SECTION;

  uint8_t p, q, t;
  uint8_t satval = (s * v) / 256;

  p = v - satval;
  q = v - (satval * subSection) / HSV_SECTION;
  t = p + (satval * subSection) / HSV_SECTION;

  switch(sect) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = t;
      g = p;
      b = v;
      break;
    default:
      r = v;
      g = p;
      b = q;
      break;
  }
  return (r << 16) | (g << 8) | (b);
}

void *createHSVImage(int w, int h) {
  uint32_t *img = (uint32_t*)malloc(sizeof(uint32_t) * w * h);
  int x, y;

  for (x = 0; x < w; x++) {
    for (y = 0; y < h; y++) {
      img[y*w + x] = hsvToRgb(x, h-y, 255);
      //printf("%d %d %x\n",x, y, img[y*w+x]);
    }
  }
  return img;
}

void createHSVBMP(UG_BMP *bmp, int w, int h) {
  bmp->p = createHSVImage(w, h);
  bmp->width = w;
  bmp->height = h;
  bmp->bpp = BMP_BPP_32;
  bmp->colors = BMP_RGB888;
}
