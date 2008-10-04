#ifndef PHONEGUI_INIT_H
#define PHONEGUI_INIT_H

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <stdlib.h>
#include "pipe.h"

void (*phonegui_input_callback)(void *data, Evas_Object *obj, const char *emission, const char *source);
void (*phonegui_event_callback)(int event);

PipeHandler pipe_handler;

Ecore_Evas   *ee;
Evas         *evas;
Evas_Object  *edje;
double       edje_w, edje_h;

void phonegui_init(int argc, char **argv);
void ui_init();
void ui_input(void *data, Evas_Object *obj, const char *emission, const char *source);
int ui_event(void *data, Ecore_Fd_Handler *fdh);

#endif
