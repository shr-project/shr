#include "phonegui-message.h"
#include <string.h>
#include <dbus/dbus-glib.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include "phonegui-init.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/incoming-message.edj"

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
    tmp_id = id;
    pipe_write(pipe_handler, message_event, EVENT_SHOW);
}

void phonegui_message_hide() {
    g_debug("phonegui_message_hide()");
    pipe_write(pipe_handler, message_event, EVENT_HIDE);
}

void message_delete(Ecore_Evas *ee) {
    g_debug("message_delete()");
    phonegui_message_hide();
}

void message_event(int event) {
    g_debug("message_event()");

    if(event == EVENT_SHOW) {
        window_create("New SMS", message_event, message_delete);
        elm_layout_file_set(layout, UI_FILE, "message");
        evas_object_show(win);
        ogsmd_sim_retrieve_message(tmp_id, retrieve_callback, NULL);
    } else if(event == EVENT_MODE_MESSAGE) {
        edje_object_part_text_set(elm_layout_edje_get(layout), "number", tmp_number);
        edje_object_part_text_set(elm_layout_edje_get(layout), "content", tmp_content);
    } else if(event == EVENT_HIDE) {
        frame_hide();
        window_destroy();
    } else {
        g_error("Unknown event");
    }
}

void retrieve_callback(GError *error, char *status, char *number, char *content, GHashTable *properties, gpointer userdata) {
    g_debug("retrieve_callback()");
    tmp_number = strdup(number);
    tmp_content = strdup(content);
    tmp_properties = properties; // TODO: copy
    pipe_write(pipe_handler, message_event, EVENT_MODE_MESSAGE);
}

