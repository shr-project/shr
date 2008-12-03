#include "phonegui-messages.h"
#include <glib.h>
#include "window.h"
#include "async.h"
#include "views.h"

static struct Window *win = NULL;

static void _show(struct Window *win);
static void _hide(struct Window *win);


void phonegui_messages_show(int argc, char** argv) {
    g_debug("phonegui_messages_show()");
    if(win == NULL) {
        win = window_new("Messages");
        async_trigger(_show, win);
    }
}

void phonegui_messages_hide() {
    g_debug("phonegui_messages_hide()");
    if(win != NULL) {
        async_trigger(_hide, win);
        win = NULL;
    }
}


static void _show(struct Window *win) {
    g_debug("_show()");
    window_init(win);
    window_view_show(win, NULL, message_list_view_show, message_list_view_hide);
}

static void _hide(struct Window *win) {
    g_debug("_hide()");
    window_destroy(win, NULL);
}

