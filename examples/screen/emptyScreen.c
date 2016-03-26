#include "ugui.h"
#include "simulation.h"

UG_GUI gui;
#define MAX_OBJS 5


void GUI_Setup(void *pset, int w, int h) {
  //Setup the system
  UG_Init(&gui, pset, w, h);
  UG_SetBackcolor(C_BLACK);
  UG_SetForecolor(C_WHITE);
}

void GUI_Process() {
  //Empty Screen
  UG_Update();
}
