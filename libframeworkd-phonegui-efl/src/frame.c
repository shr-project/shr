#include "frame.h"
#include <stdlib.h>

static void (*frame_hide_cb)() = NULL;

void frame_show(void (*show_cb)(), void (*hide_cb)()) {
    if(frame_hide_cb != NULL)
        frame_hide_cb();

    if(hide_cb != NULL)
        frame_hide_cb = hide_cb;

    show_cb();
}


