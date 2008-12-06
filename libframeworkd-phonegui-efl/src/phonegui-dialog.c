#include "phonegui-dialog.h"
#include <glib.h>
#include <assert.h>
#include "instance.h"
#include "window.h"
#include "async.h"
#include "views.h"

static void _show(struct Window *win);
static void _hide(struct Window *win);


void phonegui_dialog_show(int type) {
    struct Window *win = window_new("Information");
    instance_manager_add(INSTANCE_DIALOG, type, win);

    async_trigger(_show, win);
}

void phonegui_dialog_hide(int type) {
    struct Window *win = instance_manager_remove(INSTANCE_DIALOG, type);
    assert(win != NULL);
    
    async_trigger(_hide, win);
}


static void _show(struct Window *win) {
    assert(win != NULL);
    window_init(win);
    window_view_show(win, NULL, dialog_view_show, dialog_view_hide);
}

static void _hide(struct Window *win) {
    window_destroy(win, NULL);
}


