#include "phonegui-outgoing-call.h"
#include <string.h>
#include <dbus/dbus-glib.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-call.h>
#include "phonegui-init.h"
#include "frame.h"

#define OUTGOING_CALL_UI_FILE "/usr/share/libframeworkd-phonegui-efl/outgoing-call.edj"
#define DIAL_PAD_UI_FILE "/usr/share/libframeworkd-phonegui-efl/dial-pad.edj"

enum OutgoingCallEvents {
    EVENT_SHOW,
    EVENT_MODE_MAIN,
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
    pipe_write(pipe_handler, EVENT_MODE_MAIN);
}

void phonegui_outgoing_call_hide(const int id) {
    g_debug("phonegui_outgoing_call_hide()");
    pipe_write(pipe_handler, EVENT_HIDE);
}

void outgoing_call_input(void *data, Evas_Object *o, const char *emission, const char *source) {
    g_debug("outgoing_call_input()");

	if(!strcmp(emission, "release")) {
        ogsmd_call_release(call_id, NULL, NULL);
    } else if(!strcmp(emission, "dtmf_show")) {
        frame_show(outgoing_call_dtmf_show, outgoing_call_dtmf_hide);
    } else if(!strcmp(emission, "dtmf_hide")) {
        frame_show(outgoing_call_main_show, NULL);
    } else {
        g_debug("Send DTMF: %s", emission);
        ogsmd_call_send_dtmf(strdup(emission), NULL, NULL); 
    }
}

void outgoing_call_event(int event) {
    g_debug("outgoing_call_event()");

    if(event == EVENT_SHOW) {
        ecore_evas_show(ee);
    } else if(event == EVENT_MODE_MAIN) {
        frame_show(outgoing_call_main_show, NULL);
    } else if(event == EVENT_HIDE) {
        ecore_evas_hide(ee);
    } else {
        g_error("Unknown event: %d", event);
    }
}

void outgoing_call_main_show() {
    edje_object_file_set(edje, OUTGOING_CALL_UI_FILE, "outgoing_call");
    edje_object_part_text_set(edje, "number", call_number);
}

void outgoing_call_dtmf_show() {
    edje_object_file_set(edje, OUTGOING_CALL_UI_FILE, "dtmf");
}

void outgoing_call_dtmf_hide() {

}

