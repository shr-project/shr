#ifndef _PHONEGUI_DIALER_H
#define _PHONEGUI_DIALER_H

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>

void phonegui_dialer_show();
void phonegui_dialer_hide();

void dialer_input(void *data, Evas_Object *obj, const char *emission, const char *source);
void dialer_event(int event);
void dialer_update();
void dialer_call_initiate_callback(GError *error, int call_id, gpointer userdata);

#endif

