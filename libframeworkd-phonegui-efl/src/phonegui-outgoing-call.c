#include "phonegui-outgoing-call.h"
#include <stdlib.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <dbus/dbus-glib.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-call.h>
#include "pipe.h"
#include "phonegui-init.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/outgoing-call.edj"

#define EVENT_SHOW "a"
#define EVENT_HIDE "b"

extern PhoneguiMode phonegui_mode;
static int call_id;


void phonegui_outgoing_call_ui_show(const int id, const int status, const char *number) {
    g_debug("phonegui_outgoing_call_ui_show()");
    phonegui_mode = MODE_OUTGOING_CALL;
    call_id = id;
    pipe_write(pipe_handler, EVENT_SHOW);
}

void phonegui_outgoing_call_ui_hide(const int id) {
    g_debug("phonegui_outgoing_call_ui_hide()");
    pipe_write(pipe_handler, EVENT_HIDE);
}



void outgoing_call_ui_input(void *data, Evas_Object *o, const char *emission, const char *source) {
    g_debug("outgoing_call_ui_input()");

	if(!strcmp(emission, "release")) {
        g_debug("call release");
        ogsmd_call_release(call_id, NULL, NULL);
    } else {
        g_error("Unknown input %s", emission);
    }
}


int outgoing_call_ui_event(void *data, Ecore_Fd_Handler *fdh) {
    g_debug("outgoing_call_ui_event()");
    char* event = pipe_read(pipe_handler);

    if(!strcmp(event, EVENT_SHOW)) {
        edje_object_file_set(edje, UI_FILE, "outgoing_call");
        ecore_evas_show(ee);
    } else if(!strcmp(event, EVENT_HIDE)) {
        ecore_evas_hide(ee);
    } else {
        g_error("unknown pipe event: %s", event);
    }

    return 1;
}

