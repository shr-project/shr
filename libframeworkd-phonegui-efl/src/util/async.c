#include "pipe.h"
#include "async.h"


static PipeHandler pipe_handler;

void async_init() {
    pipe_handler = pipe_create();
}

int async_fd_input_get()
{
    return pipe_handler.input;
}


void async_dispatch() {
    PipeMessage *m;
    while((m = pipe_read(pipe_handler)) != -1) {
        if(m->callback != NULL) {
            m->callback(m->userdata); 
        }
    }
}

void async_trigger(void (*cb)(), void *userdata) {
    pipe_write(pipe_handler, cb, userdata);
}
