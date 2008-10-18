#include "phonegui-incoming-call.h"
#include <stdlib.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <dbus/dbus-glib.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-call.h>
#include "phonegui-init.h"
#include "frame.h"

#define INCOMING_CALL_UI_FILE "/usr/share/libframeworkd-phonegui-efl/incoming-call.edj"
#define DIAL_PAD_UI_FILE "/usr/share/libframeworkd-phonegui-efl/dial-pad.edj"

enum incomingCallEvents {
    EVENT_SHOW,
    EVENT_MODE_MAIN,
    EVENT_HIDE
};

static Evas_Object *pad;

static int call_id;
static char* call_number;


void phonegui_incoming_call_show(const int id, const int status, const char *number) {
    g_debug("phonegui_incoming_call_show(): %s", number);
    phonegui_input_callback = incoming_call_input;
    phonegui_event_callback = incoming_call_event;
    call_id = id;
    call_number = number;
    pipe_write(pipe_handler, EVENT_SHOW);
    pipe_write(pipe_handler, EVENT_MODE_MAIN);
}

void phonegui_incoming_call_hide(const int id) {
    g_debug("phonegui_incoming_call_hide()");
    free(call_number);
    pipe_write(pipe_handler, EVENT_HIDE);
}

void incoming_call_input(void *data, Evas_Object *o, const char *emission, const char *source) {
    g_debug("incoming_call_input()");

    if(!strcmp(emission, "accept")) {
        ogsmd_call_activate(call_id, NULL, NULL);
        frame_show(incoming_call_accepted_show, NULL);
    } else if(!strcmp(emission, "release")) {
        ogsmd_call_release(call_id, NULL, NULL);
    } else if(!strcmp(emission, "dtmf_show")) {
        frame_show(incoming_call_dtmf_show, incoming_call_dtmf_hide);
    } else if(!strcmp(emission, "dtmf_hide")) {
        frame_show(incoming_call_accepted_show, NULL);
    } else {
        g_debug("Send DTMF: %s", emission);
        ogsmd_call_send_dtmf(strdup(emission), NULL, NULL); 
    }
}

void incoming_call_event(int event) {
    g_debug("incoming_call_event()");

    if(event == EVENT_SHOW) {
        ecore_evas_show(ee);
    } else if(event == EVENT_MODE_MAIN) {
        frame_show(incoming_call_main_show, NULL);
    } else if(event == EVENT_HIDE) {
        ecore_evas_hide(ee);
    } else {
        g_error("Unknown event: %d", event);
    }
}

void incoming_call_main_show() {
    edje_object_file_set(edje, INCOMING_CALL_UI_FILE, "incoming_call");
    edje_object_part_text_set(edje, "number", call_number);
}

void incoming_call_accepted_show() {
    edje_object_file_set(edje, INCOMING_CALL_UI_FILE, "accepted");
    edje_object_part_text_set(edje, "number", call_number);
}

void incoming_call_dtmf_show() {
    edje_object_file_set(edje, INCOMING_CALL_UI_FILE, "dtmf");

    pad = edje_object_add(evas);
    edje_object_file_set(pad, DIAL_PAD_UI_FILE, "dial-pad");
    edje_object_signal_callback_add(pad, "*", "input", ui_input, NULL);
    edje_object_part_swallow(edje, "swallow", pad);
}

void incoming_call_dtmf_hide() {
    edje_object_part_unswallow(edje, pad);
    edje_object_signal_callback_del(pad, "*", "input", ui_input);
}

