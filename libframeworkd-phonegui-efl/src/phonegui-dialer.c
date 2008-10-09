#include "phonegui-dialer.h"
#include <pthread.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-call.h>
#include "phonegui-init.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/dialer.edj"

static char number[64];
static int  number_length = 0;

enum DialerEvents {
    EVENT_SHOW,
    EVENT_HIDE
};


void phonegui_dialer_show() {
    g_debug("phonegui_dialer_show()");
    phonegui_input_callback = dialer_input;
    phonegui_event_callback = dialer_event;
    pipe_write(pipe_handler, EVENT_SHOW);
}

void phonegui_dialer_hide() {
    g_debug("phonegui_dialer_hide()");
}


void dialer_input(void *data, Evas_Object *obj, const char *emission, const char *source) {
    g_debug("dialer_input() input: %s", emission);

    if(!strcmp(emission, "call")) {
        g_debug("initiate call");
        ogsmd_call_initiate(number, "voice", dialer_call_initiate_callback, NULL);
    } else if(!strcmp(emission, "options")) {
    } else if(!strcmp(emission, "delete") && number_length > 0) {
        number_length--;
        number[number_length] = '\0';
        dialer_update();
    } else if(number_length < 64) {
        strncat(number, emission, 1);
        number_length++;
        dialer_update();
    }
}

void dialer_event(int event) {
    g_debug("dialer_event()");
    if(event == EVENT_SHOW) {
        edje_object_file_set(edje, UI_FILE, "main");
        ecore_evas_show(ee);
    }
}

void dialer_update() {
    g_debug("dialer_update()");
    edje_object_part_text_set(edje, "number", number);
}

void dialer_call_initiate_callback(GError *error, int call_id, gpointer userdata) {
    g_debug("call_initiate_callback()");

    /* Exit */
    ecore_main_loop_quit();
}


