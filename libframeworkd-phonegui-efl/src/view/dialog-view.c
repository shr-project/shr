#include "views.h"
#include <frameworkd-phonegui/frameworkd-phonegui.h>

struct DialogViewData {
    struct Window *win;
    Evas_Object *bt_close;
    int type;
};

static void dialog_view_close_clicked(void *userdata, Evas_Object *obj, void *event_info);


struct DialogViewData *dialog_view_show(struct Window *win, GHashTable *options) {
    struct DialogViewData *data = g_slice_alloc0(sizeof(struct DialogViewData));
    data->win = win;
    data->type = g_hash_table_lookup(options, "type");
    assert(data->type != NULL);

    window_layout_set(win, DIALOG_FILE, "dialog");
    if(data->type == PHONEGUI_DIALOG_MESSAGE_STORAGE_FULL)
        window_text_set(win, "content", "Your storage is full. Please delete some messages or you are not going to receive messages anymore!");
    else if(data->type == PHONEGUI_DIALOG_SIM_NOT_PRESENT)
        window_text_set(win, "content", "GSM is not available, because no SIM card is present.");
    else
        window_text_set(win, "content", "Unknown message.");

    data->bt_close = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt_close, "Close");
    evas_object_smart_callback_add(data->bt_close, "clicked", dialog_view_close_clicked, data);
    window_swallow(win, "button_close", data->bt_close);
    evas_object_show(data->bt_close);


    window_show(win);
    return data;
}

void dialog_view_hide(struct DialogViewData *data) {
    evas_object_del(data->bt_close);
}

static void dialog_view_close_clicked(void *userdata, Evas_Object *obj, void *event_info) {
    struct DialogViewData *data = (struct DialogViewData *) userdata;
    window_destroy(data->win, NULL);
}

