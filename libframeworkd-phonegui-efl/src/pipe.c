#include "pipe.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>

PipeHandler pipe_create() {
    int r;
    int fds[2];

    r = pipe(fds);
    if(r == -1) {
        g_error("pipe() failed");
    }
    fcntl(fds[0], F_SETFL, fcntl(fds[0], F_GETFL) | O_NONBLOCK);

    PipeHandler h;
    h.input = fds[0];
    h.output = fds[1];

    return h;
}

int pipe_read(PipeHandler h) {
    int r, event;
    r = read(h.input, &event, sizeof(event));
    if(r == -1)
        return -1;
    else
        return event;
}

void pipe_write(PipeHandler h, int event) {
    write(h.output, &event, sizeof(event));
}


