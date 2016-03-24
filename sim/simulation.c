#include "simulation.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "x11-driver.h"

#define WIDTH 480
#define HEIGHT 272

void main (void) {
  printf("Simulation Application");

  //Setup X
  if (true != x11_setup(WIDTH, HEIGHT)) {
    printf("Error Initializing x11 driver");
  }
  //Setup UGUI
  GUI_Setup(x11_getPsetFunctionPointer(), WIDTH, HEIGHT);

  while (true) {
    GUI_Process();
    x11_process();
    usleep(100000);
  }

}
