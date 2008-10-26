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

Ecore_Evas   *ee;
Evas         *evas;
Evas_Object  *edje;
double       edje_w, edje_h;

void phonegui_init(int argc, char **argv, void (*exit_cb)());
int event_callback(void *data, Ecore_Fd_Handler *fdh);

void window_create();
void window_resize_callback(Evas *ev);

#endif
