#include "views.h"


struct UssdViewData {
    struct Window *win;
    int mode;
    char *message;
    Evas_Object *bt_close;

    void (*callback_close)(void *data);
    void *callback_close_data;
};


static void frame_ussd_show(struct UssdViewData *data);
static void frame_ussd_hide(struct UssdViewData *data);
static void frame_ussd_close_clicked(struct UssdViewData *data, Evas_Object *obj, void *event_info);
//static void frame_ussd_keypad_clicked(struct UssdViewData *data, Evas_Object *obj, void *event_info);


struct UssdViewData *ussd_view_show(struct Window *win, GHashTable *options) {
    struct UssdViewData *data = g_slice_alloc0(sizeof(struct UssdViewData));
    data->win = win;
    
    assert(options != NULL);
    data->mode = (int) g_hash_table_lookup(options, "mode");
    data->message = string_replace_with_tags(g_hash_table_lookup(options, "message"));
    data->callback_close = g_hash_table_lookup(options, "callback_close");
    data->callback_close_data = g_hash_table_lookup(options, "callback_close_data");  

    window_frame_show(win, data, frame_ussd_show, frame_ussd_hide);
    window_show(win);
    return data;
}

void ussd_view_hide(struct UssdViewData *data) {
    if(data->callback_close != NULL)
        data->callback_close(data->callback_close_data);
    //free(data->message); // TODO: Why does this produce a segfault and have borked data?
    g_slice_free(struct UssdViewData, data);
}


/*
 * Frame "ussd"
 */

static void frame_ussd_show(struct UssdViewData *data) {
    struct Window *win = data->win;
    window_layout_set(win, USSD_FILE, "ussd");
    window_text_set(win, "mode", "Data:");
    window_text_set(win, "message", data->message);

    data->bt_close = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt_close, "Close");
    evas_object_smart_callback_add(data->bt_close, "clicked", frame_ussd_close_clicked, data);
    window_swallow(win, "button_close", data->bt_close);
    evas_object_show(data->bt_close);

    /*
    data->keypad = elm_keypad_add(window_evas_object_get(win));
    evas_object_smart_callback_add(data->keypad, "clicked", frame_ussd_keypad_clicked, data);
    window_swallow(win, "keypad", data->keypad);
    evas_object_show(data->keypad);
    */
}

static void frame_ussd_hide(struct UssdViewData *data) {
    struct Window *win = data->win;
    evas_object_del(data->bt_close);

    /*
    window_unswallow(win, data->keypad);
    evas_object_smart_callback_del(data->keypad, "clicked", frame_ussd_keypad_clicked);
    evas_object_del(data->keypad);
    */
}

static void frame_ussd_close_clicked(struct UssdViewData *data, Evas_Object *obj, void *event_info) {
    window_destroy(data->win, NULL);
}

/*
static void frame_ussd_keypad_clicked(struct UssdViewData *data, Evas_Object *obj, void *event_info) {
    char input = (char) event_info;

    if(data->number_length < 64) {
        strncat(data->number, &input, 1);
        data->number_length++;
        frame_ussd_number_update(data);
    }

    if(data->number_length == 1) {
        edje_object_signal_emit(elm_layout_edje_get(window_layout_get(data->win)), "number_available", "elm");
    }
}
*/

