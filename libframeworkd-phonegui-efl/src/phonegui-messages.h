#ifndef _PHONEGUI_messages_H
#define _PHONEGUI_messages_H

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>

typedef enum {
    MODE_MAIN,
    MODE_INBOX,
    MODE_OUTBOX,
    MODE_VIEW,
    MODE_NEW
} MessagesMode;

void phonegui_messages_launch(int argc, char** argv);

void messages_ui_input(void *data, Evas_Object *obj, const char *emission, const char *source);
void messages_ui_event(void *data, Ecore_Fd_Handler *fdh);
void messages_ui_update();
void messages_call_initiate_callback(GError *error, int call_id, gpointer userdata);

#endif

