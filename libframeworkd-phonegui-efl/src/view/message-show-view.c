#include "views.h"
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>


struct MessageShowViewData {
    struct Window *win;
    int id;
    char *status, *number, *content;
    GHashTable *properties;
    GValueArray *message;
    Evas_Object *content_entry, *bt1, *bt2, *bt3, *hv, *bx, *hbt1, *hbt2, *hbt3;

    void (*callback)();
    void *callback_data;
};

typedef enum {
    MODE_FOLDERS,
    MODE_LIST,
    MODE_MESSAGE,
    MODE_NEW1,
    MODE_NEW2,
    MODE_DELETE
} MessagesModes;


static void retrieve_callback(GError *error, char *status, char *number, char *content, GHashTable *properties, struct MessageShowViewData *data);
static void retrieve_callback2(struct MessageShowViewData *data);
static void message_show_view_close_clicked(struct MessageShowViewData *data, Evas_Object *obj, void *event_info);
static void message_show_view_answer_clicked(struct MessageShowViewData *data, Evas_Object *obj, void *event_info);
static void message_show_view_delete_clicked(struct MessageShowViewData *data, Evas_Object *obj, void *event_info);
static void message_show_view_delete_callback(struct MessageShowViewData *data);
static void message_show_view_delete_callback_callback(struct MessageShowViewData *data);
static void my_hover_bt_1(void *data, Evas_Object *obj, void *event_info);



struct MessageShowViewData *message_show_view_show(struct Window *win, GHashTable *options) {
    assert(win != NULL);
    assert(options != NULL);

    struct MessageShowViewData *data = g_slice_alloc0(sizeof(struct MessageShowViewData));
    data->win = win;
    data->id = g_hash_table_lookup(options, "id");

    if(options != NULL) {
        data->callback = g_hash_table_lookup(options, "delete_callback"); 
        data->callback_data = g_hash_table_lookup(options, "delete_callback_data"); 
    }

    ogsmd_sim_retrieve_message(data->id, retrieve_callback, data);
    return data;
}


void message_show_view_hide(struct MessageShowViewData *data) {
    struct Window *win = data->win;
    evas_object_del(data->bt1);
    evas_object_del(data->bt2);
    evas_object_del(data->bt3);
    evas_object_del(data->hbt1);
    evas_object_del(data->bx);
    evas_object_del(data->hv);
    g_slice_free(struct MessageShowViewData, data);
}




static void retrieve_callback(GError *error, char *status, char *number, char *content, GHashTable *properties, struct MessageShowViewData *data) {
    g_debug("retrieve_callback()");
    data->status = strdup(status);
    data->number = strdup(number);
    data->content = strdup(content);
    data->properties = properties; // TODO: copy

    async_trigger(retrieve_callback2, data);
}


static void retrieve_callback2(struct MessageShowViewData *data) {
    struct Window *win = data->win;
    window_layout_set(win, MESSAGE_FILE, "message_show");

    char *timestr = g_value_get_string(g_hash_table_lookup(data->properties, "timestamp"));
    time_t timestamp = time_stringtotimestamp(timestr);
    char *status = data->status;
    char *number = data->number;
    char *content = string_replace_with_tags(data->content);

    char datestr[32];
    strftime(datestr, 31, "%d.%m.%Y, %H:%M", gmtime(&timestamp));

    window_text_set(win, "text_status", status);
    window_text_set(win, "text_number", number);
    window_text_set(win, "text_content", content);
    window_text_set(win, "text_date", datestr);

    data->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt1, "Close");
    evas_object_smart_callback_add(data->bt1, "clicked", message_show_view_close_clicked, data);
    window_swallow(win, "button_close", data->bt1);
    evas_object_show(data->bt1);


    // Options button with hover
    data->hv = elm_hover_add(window_evas_object_get(win));

    data->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt2, "Options");
    evas_object_smart_callback_add(data->bt2, "clicked", my_hover_bt_1, data->hv);
    window_swallow(win, "button_options", data->bt2);
    evas_object_show(data->bt2);

    elm_hover_parent_set(data->hv, window_evas_object_get(win));
    elm_hover_target_set(data->hv, data->bt2);

    data->bx = elm_box_add(window_evas_object_get(win));
    elm_box_horizontal_set(data->bx, 1);
    elm_box_homogenous_set(data->bx, 1);
    evas_object_show(data->bx);

    data->hbt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->hbt1, "Delete");
    evas_object_size_hint_min_set(data->hbt1, 130, 80);
    evas_object_smart_callback_add(data->hbt1, "clicked", message_show_view_delete_clicked, data);
    evas_object_show(data->hbt1);
    elm_box_pack_end(data->bx, data->hbt1);

    elm_hover_content_set(data->hv, "top", data->bx);


    data->bt3 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt3, "Answer");
    evas_object_smart_callback_add(data->bt3, "clicked", message_show_view_answer_clicked, data);
    window_swallow(win, "button_answer", data->bt3);
    evas_object_show(data->bt3);


    window_show(win);
}




static void message_show_view_close_clicked(struct MessageShowViewData *data, Evas_Object *obj, void *event_info) {
    window_destroy(data->win, NULL);
}

static void message_show_view_answer_clicked(struct MessageShowViewData *data, Evas_Object *obj, void *event_info) {
    GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(options, "recipient", data->number);

    struct Window *win = window_new("Compose SMS");
    window_init(win);
    window_view_show(win, options, message_new_view_show, message_new_view_hide);
}

static void message_show_view_delete_clicked(struct MessageShowViewData *data, Evas_Object *obj, void *event_info) {
    GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(options, "id", data->id);
    g_hash_table_insert(options, "delete_callback", message_show_view_delete_callback);
    g_hash_table_insert(options, "delete_callback_data", data);

    struct Window *win = window_new("Delete Message");
    window_init(win);
    window_view_show(win, options, message_delete_view_show, message_delete_view_hide);
}

static void message_show_view_delete_callback(struct MessageShowViewData *data) {
    async_trigger(message_show_view_delete_callback_callback, data);
}

static void message_show_view_delete_callback_callback(struct MessageShowViewData *data) {
    window_destroy(data->win, NULL);

    if(data->callback != NULL) {
        data->callback(data->callback_data);
    }    
}

static void my_hover_bt_1(void *data, Evas_Object *obj, void *event_info) {
   Evas_Object *hv = data;
   evas_object_show(hv);
}
