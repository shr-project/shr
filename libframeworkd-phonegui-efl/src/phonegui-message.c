#include "phonegui-message.h"
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <dbus/dbus-glib.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include "pipe.h"


void phonegui_message_ui_show(const int id) {
    g_debug("phonegui_message_ui_show()");
}

void phonegui_message_ui_hide() {
    g_debug("phonegui_message_ui_hide()");
}


