#include "phonegui-sim-auth.h"
#include <glib.h>
#include <assert.h>
#include "window.h"
#include "views.h"
#include "async.h"

static struct Window *win = NULL;

static void _show(GHashTable * options);
static void _hide(struct Window *win);


void
phonegui_backend_sim_auth_show(const int status)
{
	g_debug("phonegui_backend_sim_auth_show()");

	if (win == NULL) {
		win = window_new(D_("SIM Auth"));

		GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
		g_hash_table_insert(options, "win", win);
		g_hash_table_insert(options, "status", GINT_TO_POINTER(status));
		async_trigger(_show, options);
	}
}

void
phonegui_backend_sim_auth_hide(const int status)
{
	g_debug("phonegui_backend_sim_auth_hide()");

	/*
	 * The status variable is not needed here, because the view
	 * stores the status itself and knows which input type (PIN / PUK)
	 * was correct or wrong when the "hide" function is called
	 */

	if (win != NULL) {
		async_trigger(_hide, win);
		win = NULL;
	}
}


static void
_show(GHashTable * options)
{
	g_debug("_show()");

	struct Window *win = g_hash_table_lookup(options, "win");
	assert(win != NULL);

	window_init(win);
	window_view_show(win, options, sim_auth_input_view_show,
			 sim_auth_input_view_hide);
	window_show(win);
}

static void
_hide(struct Window *win)
{
	g_debug("_hide()");
	window_destroy(win, NULL);
}
