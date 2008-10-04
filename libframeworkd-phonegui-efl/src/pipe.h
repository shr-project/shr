#ifndef _PIPE_H
#define _PIPE_H

#include <fcntl.h>

typedef struct {
    int input;
    int output;
} PipeHandler;


PipeHandler pipe_create();
int pipe_read(PipeHandler h);
void pipe_write(PipeHandler h, int data);

#endif
