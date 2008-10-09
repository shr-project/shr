#include "phonegui-message.h"
#include <string.h>
#include <dbus/dbus-glib.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include "phonegui-init.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/incoming-message.edj"

// TODO: Remove tmp vars
static int tmp_id;
static char *tmp_number;
static char *tmp_content;
static GHashTable *tmp_properties;

enum MessageEvents {
    EVENT_SHOW,
    EVENT_MODE_MESSAGE,
    EVENT_HIDE
};


void phonegui_message_show(const int id) {
    g_debug("phonegui_message_show()");
    phonegui_input_callback = message_input;
    phonegui_event_callback = message_event;
    tmp_id = id;
    pipe_write(pipe_handler, EVENT_SHOW);
}

void phonegui_message_hide() {
    g_debug("phonegui_message_hide()");
    pipe_write(pipe_handler, EVENT_HIDE);
}


void message_input(void *data, Evas_Object *obj, const char *emission, const char *source) {
    g_debug("message_input()");
}

void message_event(int event) {
    g_debug("message_event()");

    if(event == EVENT_SHOW) {
        edje_object_file_set(edje, UI_FILE, "message");
        ecore_evas_show(ee);
        ogsmd_sim_retrieve_message(tmp_id, retrieve_callback, NULL);
    } else if(event == EVENT_MODE_MESSAGE) {
        edje_object_part_text_set(edje, "number", tmp_number);
        edje_object_part_text_set(edje, "content", tmp_content);
    } else if(event == EVENT_HIDE) {
        ecore_evas_hide(ee);
    } else {
        g_error("Unknown event");
    }
}


void retrieve_callback(GError *error, char *status, char *number, char *content, GHashTable *properties, gpointer userdata) {
    g_debug("retrieve_callback()");
    tmp_number = strdup(number);
    tmp_content = strdup(content);
    tmp_properties = properties; // TODO: copy
    pipe_write(pipe_handler, EVENT_MODE_MESSAGE);
}

