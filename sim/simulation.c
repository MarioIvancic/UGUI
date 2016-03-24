#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "x11-driver.h"

void main (void) {
  printf("Simulation Application");

  if (true != x11_setup(480, 272)) {
    printf("Error Initializing x11 driver");
  }

}
