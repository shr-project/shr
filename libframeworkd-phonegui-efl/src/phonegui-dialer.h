#ifndef _PHONEGUI_DIALER_H
#define _PHONEGUI_DIALER_H

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>

void phonegui_dialer_launch(int argc, char** argv);

void dialer_ui_input(void *data, Evas_Object *obj, const char *emission, const char *source);
void dialer_ui_event(void *data, Ecore_Fd_Handler *fdh);
void dialer_ui_update();
void dialer_call_initiate_callback(GError *error, int call_id, gpointer userdata);

#endif

