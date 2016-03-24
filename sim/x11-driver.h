#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

#include "ugui.h"

bool x11_setup(int width, int height);

void *x11_getPsetFunctionPointer();

void x11_process();
