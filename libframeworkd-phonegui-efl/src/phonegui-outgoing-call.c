#include "phonegui-outgoing-call.h"
#include <string.h>
#include <dbus/dbus-glib.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-call.h>
#include "phonegui-init.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/outgoing-call.edj"

enum OutgoingCallEvents {
    EVENT_SHOW,
    EVENT_HIDE
};

static int call_id;
static char* call_number;


void phonegui_outgoing_call_show(const int id, const int status, const char *number) {
    g_debug("phonegui_outgoing_call_show()");
    phonegui_input_callback = outgoing_call_input;
    phonegui_event_callback = outgoing_call_event;
    call_id = id;
    call_number = number;
    pipe_write(pipe_handler, EVENT_SHOW);
}

void phonegui_outgoing_call_hide(const int id) {
    g_debug("phonegui_outgoing_call_hide()");
    pipe_write(pipe_handler, EVENT_HIDE);
}

void outgoing_call_input(void *data, Evas_Object *o, const char *emission, const char *source) {
    g_debug("outgoing_call_input()");

	if(!strcmp(emission, "release")) {
        g_debug("call release");
        ogsmd_call_release(call_id, NULL, NULL);
    } else {
        g_error("Unknown input %s", emission);
    }
}

void outgoing_call_event(int event) {
    g_debug("outgoing_call_event()");

    if(event == EVENT_SHOW) {
        edje_object_file_set(edje, UI_FILE, "outgoing_call");
        edje_object_part_text_set(edje, "number", call_number);
        ecore_evas_show(ee);
    } else if(event == EVENT_HIDE) {
        g_debug("hide");
        ecore_evas_hide(ee);
    } else {
        g_error("Unknown event: %d", event);
    }
}

