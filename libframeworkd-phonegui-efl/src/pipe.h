#ifndef _PIPE_H
#define _PIPE_H

#include <fcntl.h>

typedef struct {
    int input;
    int output;
} PipeHandler;

typedef struct {
    void (*callback)(int event);
    int event;
} PipeMessage;


PipeHandler pipe_create();
PipeMessage* pipe_read(PipeHandler h);
void pipe_write(PipeHandler h, void (*event_cb)(int event), int data);

#endif
