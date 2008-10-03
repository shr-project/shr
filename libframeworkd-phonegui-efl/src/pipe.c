#include "pipe.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>

PipeHandler pipe_create()
{
    int r;
    int fds[2];

    r = pipe(fds);
    if(r == -1)
    {
        g_debug("pipe() failed");
        exit(-1);
    }
    fcntl(fds[0], F_SETFL, fcntl(fds[0], F_GETFL) | O_NONBLOCK);

    PipeHandler h;
    h.input = fds[0];
    h.output = fds[1];

    return h;
}


char* pipe_read(PipeHandler h)
{
    /*
    int fd;
    char event[2];
    fd = ecore_main_fd_handler_fd_get(fdh);

    // Reads all events and use only one
    while(read (fd, event, sizeof(event) - 1) > 0);
    event[sizeof(event) - 1] = '\0';
    */

    char* event = g_malloc(2);
    while(read(h.input, event, 1) > 0);
    event[1] = '\0';

    return event;
}

void pipe_write(PipeHandler h, char* data)
{
    write(h.output, data, strlen(data));
}


