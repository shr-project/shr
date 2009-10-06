#ifndef _PIPE_H
#define _PIPE_H

#include <glib.h>
#include <fcntl.h>

typedef struct {
	int input;
	int output;
} PipeHandler;

typedef struct {
	void (*callback) ();
	void *userdata;
} PipeMessage;


PipeHandler pipe_create();
PipeMessage *pipe_read(PipeHandler h);
void pipe_write(PipeHandler h, void (*callback) (), void *userdata);

#endif
