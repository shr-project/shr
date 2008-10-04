#include "phonegui-message.h"
#include <dbus/dbus-glib.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include "phonegui-init.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/message.edj"

enum MessageEvents {
    EVENT_SHOW,
    EVENT_HIDE
};

void phonegui_message_show(const int id) {
    g_debug("phonegui_message_show()");
    phonegui_input_callback = message_input;
    phonegui_event_callback = message_event;
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
}
