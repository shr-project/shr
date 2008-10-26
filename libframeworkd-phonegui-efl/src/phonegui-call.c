#include "phonegui-call.h"
#include <stdlib.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <dbus/dbus-glib.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-call.h>
#include "phonegui-init.h"
#include "frame.h"

#define CALL_UI_FILE "/usr/share/libframeworkd-phonegui-efl/call.edj"
#define DIAL_PAD_UI_FILE "/usr/share/libframeworkd-phonegui-efl/dial-pad.edj"

enum CallEvents {
    EVENT_SHOW,
    EVENT_MODE_INCOMING,
    EVENT_MODE_ACTIVE,
    EVENT_HIDE
};

static Evas_Object *pad;
static int call_id;
static char* call_number;


void phonegui_incoming_call_show(const int id, const int status, const char *number) {
    call_show(id, status, number, "incoming");
}

void phonegui_incoming_call_hide(const int id) {
    call_hide(id);
}

void phonegui_outgoing_call_show(const int id, const int status, const char *number) {
    call_show(id, status, number, "outgoing");
}

void phonegui_outgoing_call_hide(const int id) {
    call_hide(id);
}


void call_show(const int id, const int status, const char *number, const char *type) {
    g_debug("call_show()");
    call_id = id;
    call_number = number;
    pipe_write(pipe_handler, call_event, EVENT_SHOW);
    if(!strcmp(type, "incoming")) {
        pipe_write(pipe_handler, call_event, EVENT_MODE_INCOMING);
    } else {
        pipe_write(pipe_handler, call_event, EVENT_MODE_ACTIVE);
    }
}

void call_hide(const int id) {
    g_debug("call_hide()");
    free(call_number);
    pipe_write(pipe_handler, call_event, EVENT_HIDE);
}


void call_input(void *data, Evas_Object *o, const char *emission, const char *source) {
    g_debug("call_input()");

    if(!strcmp(emission, "accept")) {
        g_debug("accept");
        ogsmd_call_activate(call_id, NULL, NULL);
        frame_show(call_active_show, NULL);
    } else if(!strcmp(emission, "release")) {
        g_debug("release");
        ogsmd_call_release(call_id, NULL, NULL);
        ecore_evas_hide(ee);
    } else if(!strcmp(emission, "dtmf_show")) {
        g_debug("dtmf show");
        frame_show(call_dtmf_show, call_dtmf_hide);
    } else if(!strcmp(emission, "dtmf_hide")) {
        g_debug("dtmf hide");
        frame_show(call_active_show, NULL);
    } else {
        g_debug("Send DTMF: %s", emission);
        ogsmd_call_send_dtmf(strdup(emission), NULL, NULL); 
    }
}

void call_delete(Ecore_Evas *ee) {
    g_debug("incoming_call_delete");
    pipe_write(pipe_handler, call_event, EVENT_HIDE);
}

void call_event(int event) {
    g_debug("call_event()");

    if(event == EVENT_SHOW) {
        g_debug("show");
        window_create("Call", call_input, call_event, call_delete);
    } else if(event == EVENT_MODE_INCOMING) {
        g_debug("mode incoming");
        frame_show(call_incoming_show, NULL);
        ecore_evas_show(ee);
    } else if(event == EVENT_MODE_ACTIVE) {
        g_debug("mode active");
        frame_show(call_active_show, NULL);
        ecore_evas_show(ee);
    } else if(event == EVENT_HIDE) {
        g_debug("ecore_evas_hide!");
        ecore_evas_hide(ee);
    } else {
        g_error("Unknown event: %d", event);
    }
}

void call_incoming_show() {
    edje_object_file_set(edje, CALL_UI_FILE, "incoming_call");
    edje_object_part_text_set(edje, "number", call_number);
}

void call_active_show() {
    edje_object_file_set(edje, CALL_UI_FILE, "call");
    edje_object_part_text_set(edje, "number", call_number);
}

void call_dtmf_show() {
    edje_object_file_set(edje, CALL_UI_FILE, "dtmf"); // takes the wole screen and has a swallow part

    pad = edje_object_add(evas); // edje object that should be swallowed
    edje_object_file_set(pad, DIAL_PAD_UI_FILE, "dial-pad"); 
    edje_object_signal_callback_add(pad, "*", "input", call_input, NULL);
    edje_object_part_swallow(edje, "swallow", pad); // swallow it!
}

void call_dtmf_hide() {
    g_debug("CALLING DTMF HIDEE");
    edje_object_part_unswallow(edje, pad);
    evas_object_hide(pad);
    edje_object_signal_callback_del(pad, "*", "input", call_input);
    evas_object_del(pad);
}

