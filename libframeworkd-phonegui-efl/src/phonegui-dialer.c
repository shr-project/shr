#include "phonegui-dialer.h"
#include <glib.h>
#include "window.h"
#include "async.h"
#include "views.h"

static struct Window *win = NULL;

static void _show(struct Window *win);
static void _hide(struct Window *win);


void phonegui_dialer_show() {
    g_debug("phonegui_dialer_show()");
    if(win == NULL) {
        win = window_new(D_("Dialer"));
        async_trigger(_show, win);
    }
}

void phonegui_dialer_hide() {
    g_debug("phonegui_dialer_hide()");
    if(win != NULL) {
        async_trigger(_hide, win);
    }
}


static void _show(struct Window *win) {
    g_debug("_show()");
    window_init(win);
    window_view_show(win, NULL, dialer_view_show, dialer_view_hide);
}

static void _hide(struct Window *win) {
    g_debug("_hide()");
    window_destroy(win, NULL);
    win = NULL;
}

