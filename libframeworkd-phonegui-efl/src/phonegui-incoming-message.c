#include "phonegui-incoming-message.h"
#include <glib.h>
#include <assert.h>
#include "instance.h"
#include "window.h"
#include "async.h"
#include "views.h"

static void _show(GHashTable * options);
static void _hide(struct Window *win);


void
phonegui_backend_message_show(const int id)
{
	g_debug("phonegui_backend_message_show(id=%d)", id);

	struct Window *win = window_new(D_("New Message"));
	instance_manager_add(INSTANCE_INCOMING_MESSAGE, id, win);

	GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(options, "win", win);
	g_hash_table_insert(options, "id", GINT_TO_POINTER(id));
	async_trigger(_show, options);
}

void
phonegui_backend_message_hide(int id)
{
	g_debug("phonegui_backend_message_hide()");
	struct Window *win =
		instance_manager_remove(INSTANCE_INCOMING_MESSAGE, id);
	async_trigger(_hide, win);
}


void
_show(GHashTable * options)
{
	g_debug("_show()");

	struct Window *win = g_hash_table_lookup(options, "win");
	assert(win != NULL);

	window_init(win);
	window_view_show(win, options, message_show_view_show,
			 message_show_view_hide);
}

void
_hide(struct Window *win)
{
	g_debug("_hide()");
	assert(win != NULL);
	window_destroy(win, NULL);
}
