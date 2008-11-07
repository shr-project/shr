#include "phonegui-call.h"
#include <stdlib.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <dbus/dbus-glib.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-call.h>
#include <frameworkd-glib/odeviced/frameworkd-glib-odeviced-audio.h>
#include "phonegui-init.h"
#include "frame.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/call.edj"

enum CallEvents {
    EVENT_SHOW,
    EVENT_MODE_INCOMING,
    EVENT_MODE_ACTIVE,
    EVENT_HIDE
};

static Evas_Object *bt1, *bt2, *bt3, *keypad;
static int call_id;
static char* call_number;
static gboolean dtmf_active = FALSE;
static gboolean speaker_active = FALSE;


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
    //free(call_number);
    pipe_write(pipe_handler, call_event, EVENT_HIDE);

    if(speaker_active) {
        call_speaker_disable();
    }
}

void call_delete(Ecore_Evas *ee) {
    g_debug("call_delete(), release call!");
    ogsmd_call_release(call_id, NULL, NULL);
    pipe_write(pipe_handler, call_event, EVENT_HIDE);
}

void call_event(int event) {
    g_debug("call_event()");

    if(event == EVENT_SHOW) {
        g_debug("show");
        window_create("Call", call_event, call_delete);
    } else if(event == EVENT_MODE_INCOMING) {
        g_debug("mode incoming");
        frame_show(call_incoming_show, call_incoming_hide);
        evas_object_show(win);
    } else if(event == EVENT_MODE_ACTIVE) {
        g_debug("mode active");
        frame_show(call_active_show, call_active_hide);
        g_debug("call_active_show_done!");
        evas_object_show(win);
        g_debug("show evas object done!");
    } else if(event == EVENT_HIDE) {
        g_debug("ecore_evas_hide!");
        frame_hide();
        evas_object_hide(win);
        window_destroy();
    } else {
        g_error("Unknown event: %d", event);
    }
}


/*
 * Button triggers
 */

void call_button_keypad_clicked(void *data, Evas_Object *obj, void *event_info) {
    char string[2];
    string[0] = (char) event_info;
    string[1] = '\0';
    g_debug("call_button_keypad_clicked(): %s", string);
    ogsmd_call_send_dtmf(strdup(string), NULL, NULL);
}

void call_button_accept_clicked() {
    g_debug("accept_clicked()");
    ogsmd_call_activate(call_id, NULL, NULL);
    frame_show(call_active_show, call_active_hide);
}

void call_button_release_clicked() {
    g_debug("release_clicked()");
    ogsmd_call_release(call_id, NULL, NULL);
}

void call_button_speaker_clicked() {
    g_debug("speaker_clicked()");
    if(speaker_active) {
        speaker_active = FALSE;
        call_speaker_enable();
        edje_object_part_text_set(elm_layout_edje_get(layout), "text_speaker", "Speaker");
    } else {
        speaker_active = TRUE;
        call_speaker_disable();
        edje_object_part_text_set(elm_layout_edje_get(layout), "text_speaker", "No Speaker");
    }
}

void call_button_dtmf_clicked() {
    g_debug("dtmf_clicked()");
    if(dtmf_active) {
        dtmf_active = FALSE;
        call_dtmf_disable();
        edje_object_part_text_set(elm_layout_edje_get(layout), "text_dtmf", "Show Keypad");
    } else {
        dtmf_active = TRUE;
        call_dtmf_enable();
        edje_object_part_text_set(elm_layout_edje_get(layout), "text_dtmf", "Hide Keypad");
    }
}


/*
 * Views
 */
void call_incoming_show() {
    elm_layout_file_set(layout, UI_FILE, "incoming_call");
    edje_object_part_text_set(elm_layout_edje_get(layout), "number", call_number);

    bt1 = elm_button_add(win);
    elm_button_label_set(bt1, "Accept");
    evas_object_smart_callback_add(bt1, "clicked", call_button_accept_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_accept", bt1);
    evas_object_show(bt1);

    bt2 = elm_button_add(win);
    elm_button_label_set(bt2, "Release");
    evas_object_smart_callback_add(bt2, "clicked", call_button_release_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_release", bt2);
    evas_object_show(bt2);
}

void call_incoming_hide() {
    edje_object_part_unswallow(elm_layout_edje_get(layout), bt1);
    evas_object_del(bt1);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt2);
    evas_object_del(bt2);
}

void call_active_show() {
    elm_layout_file_set(layout, UI_FILE, "call");
    edje_object_part_text_set(elm_layout_edje_get(layout), "number", call_number);

    bt1 = elm_button_add(win);
    elm_button_label_set(bt1, "Release");
    evas_object_smart_callback_add(bt1, "clicked", call_button_release_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_release", bt1);
    evas_object_show(bt1);

    bt2 = elm_button_add(win);
    elm_button_label_set(bt2, "Speaker");
    evas_object_smart_callback_add(bt2, "clicked", call_button_speaker_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_speaker", bt2);
    evas_object_show(bt2);

    bt3 = elm_button_add(win);
    elm_button_label_set(bt3, "Keypad");
    evas_object_smart_callback_add(bt3, "clicked", call_button_dtmf_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_dtmf", bt3);
    evas_object_show(bt3);
}

void call_active_hide() {
    g_debug("call_active_hide()");
    if(dtmf_active) {
        call_dtmf_disable();
    }

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt1);
    evas_object_del(bt1);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt2);
    evas_object_del(bt2);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt3);
    evas_object_del(bt3);
}



void call_dtmf_enable() {
    g_debug("call_dtmf_enable()");
    keypad = elm_keypad_add(win);
    evas_object_smart_callback_add(keypad, "clicked", call_button_keypad_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "keypad", keypad);
    evas_object_show(keypad);
}

void call_dtmf_disable() {
    g_debug("call_dtmf_disable()");
    edje_object_part_unswallow(elm_layout_edje_get(layout), keypad);
    evas_object_smart_callback_del(keypad, "clicked", call_button_keypad_clicked);
    evas_object_del(keypad);
}

void call_speaker_enable() {
    g_debug("call_speaker_enable()");
    odeviced_audio_pull_scenario(NULL, NULL);
}

void call_speaker_disable() {
    g_debug("call_speaker_disable()");
    odeviced_audio_push_scenario("gsmspeakerout", NULL, NULL);
}

