#include "phonegui-contacts.h"
#include <glib.h>
#include "window.h"
#include "async.h"
#include "views.h"

static struct Window *win = NULL;

static void _show(struct Window *win);
static void _hide(struct Window *win);


void phonegui_contacts_show() {
    g_debug("phonegui_contacts_show()");
    if(win == NULL) {
        win = window_new(D_("Contacts"));
        async_trigger(_show, win);
    }
}

void phonegui_contacts_hide() {
    async_trigger(_hide, win);
}


static void _show(struct Window *win) {
    window_init(win);
    window_view_show(win, NULL, contact_list_view_show, contact_list_view_hide);
}

static void _hide(struct Window *win) {
    window_destroy(win, NULL);
}

