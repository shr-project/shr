#include "views.h"

struct MessageReceiptViewData {
    struct Window *win;
    char *sender_number;
    char *content;
    GHashTable *properties;
    Evas_Object *bt_close;

    void (*callback_close)(void *data);
    void *callback_close_data;
};


static void frame_message_receipt_show(void *_data);
static void frame_message_receipt_hide(void *_data);
static void frame_message_receipt_close_clicked(void *data, Evas_Object *obj, void *event_info);

struct MessageReceiptViewData *message_receipt_view_show(struct Window *win, GHashTable *options)
{
    struct MessageReceiptViewData *data = g_slice_alloc0(sizeof(struct MessageReceiptViewData));
    data->win = win;

    assert(options != NULL);
    data->sender_number = g_hash_table_lookup(options, "sender_number");
    data->content = string_replace_with_tags(g_hash_table_lookup(options, "content"));
    data->properties = g_hash_table_lookup(options, "properties");
    data->callback_close = g_hash_table_lookup(options, "callback_close");
    data->callback_close_data = g_hash_table_lookup(options, "callback_close_data");

    window_frame_show(win, data, frame_message_receipt_show, frame_message_receipt_hide);
    window_show(win);
    return (data);
}


void message_receipt_view_hide(struct MessageReceiptViewData *data)
{
    if( data->callback_close != NULL)
        data->callback_close(data->callback_close_data);
    free(data->sender_number);
    free(data->content);
    g_hash_table_destroy(data->properties);
    g_slice_free(struct MessageReceiptViewData, data);
}



static void frame_message_receipt_show(void *_data)
{
    struct MessageReceiptViewData *data = (struct MessageReceiptViewData *)_data;
    struct Window *win = data->win;
    window_layout_set(win, MESSAGE_RECEIPT_FILE, "message_receipt");
    window_text_set(win, "message", data->content);

    data->bt_close = elm_button_add(windows_evas_object_get(win));
    elm_button_label_set(data->bt_close, D_("Close"));
    evas_object_smart_callback_add(data->bt_close, "clicked", frame_message_receipt_close_clicked, data);
    window_swallow(win, "button_close", data->bt_close);
    evas_object_show(data->bt_close);

}


static void frame_message_receipt_hide(void *_data) 
{
    struct MessageReceiptViewData *data = (struct MessageReceiptViewData *)_data;
    struct Window *win = data->win;
    evas_object_del(data->bt_close);
}


static void frame_message_receipt_close_clicked(void *_data, Evas_Object *obj, void *event_info)
{
    struct MessageReceiptViewData *data = (struct MessageReceiptViewData *)_data;
    window_destroy(data->win, NULL);
}


