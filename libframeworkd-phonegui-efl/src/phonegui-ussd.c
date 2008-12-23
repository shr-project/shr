#include "phonegui-ussd.h"
#include <glib.h>
#include "window.h"
#include "async.h"
#include "views.h"

static struct Window *win_ussd = NULL;

static void _show(GHashTable *options);
static void _hide();
static void _delete(void *data, Evas_Object *win, void *event_info);


void phonegui_ussd_show(int mode, const char *message) {
    g_debug("phonegui_ussd_show(mode=%d, message=%s)", mode, message);
    if(win_ussd == NULL) {
        win_ussd = window_new("Service Data");

        GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
        g_hash_table_insert(options, "mode", mode);
        g_hash_table_insert(options, "message", message);
        async_trigger(_show, options);
    }
}

void phonegui_ussd_hide() {
    g_debug("phonegui_ussd_hide()");
    async_trigger(_hide, NULL);
}


static void _show(GHashTable *options) {
    g_debug("_show()");
    assert(win_ussd != NULL);
    window_init(win_ussd);
    window_delete_callback_set(win_ussd, _delete);
    window_view_show(win_ussd, options, ussd_view_show, ussd_view_hide);
}

static void _hide() {
    g_debug("_hide()");
    window_destroy(win_ussd, NULL);
    win_ussd = NULL;
}

static void _delete(void *data, Evas_Object *win, void *event_info) {
    g_debug("_delete()");
    win_ussd = NULL;
}

