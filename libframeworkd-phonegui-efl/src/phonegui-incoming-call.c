#include "phonegui-incoming-call.h"
#include <stdlib.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <dbus/dbus-glib.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-call.h>
#include "pipe.h"
#include "phonegui-init.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/incoming-call.edj"

enum incomingCallEvents {
    EVENT_SHOW,
    EVENT_HIDE
};

static int call_id;


void phonegui_incoming_call_show(const int id, const int status, const char *number) {
    g_debug("phonegui_incoming_call_show()");
    phonegui_input_callback = incoming_call_input;
    phonegui_event_callback = incoming_call_event;
    call_id = id;
    pipe_write(pipe_handler, EVENT_SHOW);
}

void phonegui_incoming_call_hide(const int id) {
    g_debug("phonegui_incoming_call_hide()");
    pipe_write(pipe_handler, EVENT_HIDE);
}

void incoming_call_input(void *data, Evas_Object *o, const char *emission, const char *source) {
    g_debug("incoming_call_input()");

	if(!strcmp(emission, "release")) {
        g_debug("call release");
        ogsmd_call_release(call_id, NULL, NULL);
    } else {
        g_error("Unknown input %s", emission);
    }
}

void incoming_call_event(int event) {
    g_debug("incoming_call_event()");

    if(event == EVENT_SHOW) {
        edje_object_file_set(edje, UI_FILE, "incoming_call");
        ecore_evas_show(ee);
    } else if(event == EVENT_HIDE) {
        ecore_evas_hide(ee);
    } else {
        g_error("Unknown event: %d", event);
    }
}
