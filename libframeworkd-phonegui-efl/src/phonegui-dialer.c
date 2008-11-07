#include "phonegui-dialer.h"
#include <pthread.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-call.h>
#include "phonegui-init.h"
#include "kbd.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/dialer.edj"
#define KEYPAD_UI_FILE "/usr/share/libframeworkd-phonegui-efl/keypad.edj"

enum DialerEvents {
    EVENT_SHOW,
    EVENT_HIDE
};

static char number[64];
static int  number_length = 0;
static Evas_Object *keypad, *bt_delete, *bt_options, *bt_call;


void phonegui_dialer_show() {
    g_debug("phonegui_dialer_show()");
    pipe_write(pipe_handler, dialer_event, EVENT_SHOW);
}

void phonegui_dialer_hide() {
    g_debug("phonegui_dialer_hide()");
    pipe_write(pipe_handler, dialer_event, EVENT_HIDE);
}

void dialer_delete(void *data, Evas_Object *obj, void *event_info) {
    phonegui_dialer_hide();
}

void dialer_input(void *data, Evas_Object *obj, const char *emission, const char *source) {
    g_debug("dialer_input()");
}

void dialer_event(int event) {
    g_debug("dialer_event()");
    if(event == EVENT_SHOW) {
        window_create("Dialer", dialer_input, dialer_event, dialer_delete);
        g_debug("frame_show()");
        frame_show(dialer_main_show, dialer_main_hide);
        evas_object_show(win);
    } else if(event == EVENT_HIDE) {
        evas_object_hide(win);
        phonegui_exit_cb();
    }
}

void dialer_update() {
    g_debug("dialer_update()");
    edje_object_part_text_set(elm_layout_edje_get(layout), "number", number);
}

void dialer_call_initiate_callback(GError *error, int call_id, gpointer userdata) {
    g_debug("call_initiate_callback()");
}


/*
 * Buttons triggers
 */
void dialer_button_keypad_clicked(void *data, Evas_Object *obj, void *event_info) {
    char input = (char) event_info;
    g_debug("dialer_button_keypad_clicked(): %c", input);

    if(number_length < 64) {
        strncat(number, &input, 1);
        number_length++;
        dialer_update();
    }
}

void dialer_button_delete_clicked() {
    if(number_length > 0) {
        number_length--;
        number[number_length] = '\0';
        dialer_update();
    }
}

void dialer_button_options_clicked() {
    // TODO
}

void dialer_button_call_clicked() {
    g_debug("initiate call: %s", number);
    ogsmd_call_initiate(number, "voice", dialer_call_initiate_callback, NULL);
}


/*
 * Views
 */
void dialer_main_show() {
    elm_layout_file_set(layout, UI_FILE, "main");

    keypad = elm_keypad_add(win);
    evas_object_smart_callback_add(keypad, "clicked", dialer_button_keypad_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "keypad", keypad);
    evas_object_show(keypad);

    bt_delete = elm_button_add(win);
    elm_button_label_set(bt_delete, "Delete");
    evas_object_smart_callback_add(bt_delete, "clicked", dialer_button_delete_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_delete", bt_delete);
    evas_object_show(bt_delete);

    bt_options = elm_button_add(win);
    elm_button_label_set(bt_options, "Options");
    evas_object_smart_callback_add(bt_options, "clicked", dialer_button_options_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_options", bt_options);
    evas_object_show(bt_options);

    bt_call = elm_button_add(win);
    elm_button_label_set(bt_call, "Call");
    evas_object_smart_callback_add(bt_call, "clicked", dialer_button_call_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_call", bt_call);
    evas_object_show(bt_call);
}

void dialer_main_hide() {
    edje_object_part_unswallow(elm_layout_edje_get(layout), keypad);
    evas_object_smart_callback_del(keypad, "clicked", dialer_button_keypad_clicked);
    evas_object_del(keypad);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt_delete);
    evas_object_del(bt_delete);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt_options);
    evas_object_del(bt_options);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt_call);
    evas_object_del(bt_call);
}

