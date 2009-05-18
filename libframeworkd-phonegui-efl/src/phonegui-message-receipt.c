#include "phonegui-message-receipt.h"
#include <glib.h>
#include "window.h"
#include "async.h"
#include "views.h"

static struct Window *win_receipt = NULL;

static void _show(GHashTable *options);
static void _hide();
static void _reset();



void phonegui_message_receipt_show(const char *sender_number, const char *content, void *_properties) {
    GHashTable *properties = (GHashTable *)_properties;
    g_debug("phonegui_message_receipt_show(sender_number=%s)", sender_number);
    if(win_receipt == NULL) {
        win_receipt = window_new(D_("SMS Receipt"));

        GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
        g_hash_table_insert(options, "number", g_strdup((char *)sender_number));
        g_hash_table_insert(options, "content", g_strdup((char *)content)); /* we lose the const here */
		  g_hash_table_insert(options, "properties", properties);
        g_hash_table_insert(options, "callback_close", _reset);
        async_trigger(_show, options);
    }
}

void phonegui_message_receipt_hide() {
    g_debug("phonegui_message_receipt_hide()");
    async_trigger(_hide, NULL);
}

static void _show(GHashTable *options) {
    g_debug("_show()");
    assert(win_receipt != NULL);
    window_init(win_receipt);
    window_view_show(win_receipt, options, message_receipt_view_show, message_receipt_view_hide);
}

static void _hide() {
    g_debug("_hide()");
    window_destroy(win_receipt, NULL);
}

static void _reset() {
    g_debug("_reset()");
    win_receipt = NULL;
}

