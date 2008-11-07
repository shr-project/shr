#ifndef PHONEGUI_INIT_H
#define PHONEGUI_INIT_H

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <stdlib.h>
#include "pipe.h"

int phonegui_argc;
char **phonegui_argv;
void (*phonegui_exit_cb)();

PipeHandler pipe_handler;

Evas_Object *win, *bg, *layout;

void phonegui_init(int argc, char **argv, void (*exit_cb)());
int event_callback(void *data, Ecore_Fd_Handler *fdh);

void window_create(char *title, void (*event_cb)(int event), void (*delete_cb)(Ecore_Evas *ecore_evas));
void window_destroy();

#endif
