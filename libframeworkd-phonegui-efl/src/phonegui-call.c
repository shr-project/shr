#include "phonegui-call.h"
#include "call-common.h"
#include <glib.h>
#include <assert.h>
#include "instance.h"
#include "window.h"
#include "async.h"
#include "views.h"

enum CallTypes {
    CALL_INCOMING,
    CALL_ACTIVE
};

struct Call {
    int id;
    struct Window *win;
};

static void _show(const int id, const int status, const char *number, int type);
static void _show_async(GHashTable *options);
static void _hide(const int id);
static void _delete(void *data, Evas_Object *win, void *event_info);


void phonegui_incoming_call_show(const int id, const int status, const char *number) {
    _show(id, status, number, CALL_INCOMING);
}

void phonegui_incoming_call_hide(const int id) {
    async_trigger(_hide, id);
}

void phonegui_outgoing_call_show(const int id, const int status, const char *number) {
    _show(id, status, number, CALL_ACTIVE);
}

void phonegui_outgoing_call_hide(const int id) {
    async_trigger(_hide, id);
}


static void _show(const int id, const int status, const char *number, int type) {
    struct Window *win = window_new(D_("Call"));
    window_delete_callback_set(win, _delete);
    instance_manager_add(INSTANCE_CALL, id, win);

    GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(options, "win", win);
    g_hash_table_insert(options, "id", GINT_TO_POINTER(id));
    g_hash_table_insert(options, "status", GINT_TO_POINTER(status));
    g_hash_table_insert(options, "number", (char *)number); /* we just loose the const for now */
    g_hash_table_insert(options, "type", GINT_TO_POINTER(type));
	
    async_trigger(_show_async, options);
}

static void _show_async(GHashTable *options) {
    struct Window *win = g_hash_table_lookup(options, "win");
    assert(win != NULL);
    window_init(win);

    int type = g_hash_table_lookup(options, "type");
    int id = g_hash_table_lookup(options, "id");
    if(type == CALL_INCOMING) {
        window_view_show(win, options, call_incoming_view_show, call_incoming_view_hide);
    } else if (type == CALL_ACTIVE) {
        window_view_show(win, options, call_active_view_show, call_active_view_hide);
    } else {
        g_error("Unknown call type: %d", type);
    }

    window_show(win);
}

static void _hide(const int id) {
    g_debug("call_hide(id=%d)", id);
    call_common_active_call_remove(id);
	
    struct Window *win = instance_manager_remove(INSTANCE_CALL, id);
    assert(win != NULL);
    window_destroy(win, NULL);
}

static void _delete(void *data, Evas_Object *win, void *event_info) {
    g_debug("call_delete(), release call!");
    //ogsmd_call_release(call_id, NULL, NULL);
    //window_destroy(win, NULL);
}

