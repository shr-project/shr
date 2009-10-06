#ifndef _ASYNC_H
#define _ASYNC_H

void async_init();
int async_fd_input_get();
void async_dispatch();
void async_trigger(void (*cb) (), void *userdata);

#endif
