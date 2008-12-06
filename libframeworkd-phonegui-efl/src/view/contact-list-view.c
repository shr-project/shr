#include "views.h"
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>


struct ContactListViewData {
    struct Window *win;
    Evas_Object *list;
    Evas_Object *bx, *hv;
    Evas_Object *bt1, *bt2, *bt_options, *bt_message, *bt_edit, *bt_delete;
};

static void frame_list_show(struct ContactListViewData *data);
static void frame_list_hide(struct ContactListViewData *data);
static void frame_list_new_clicked(struct ContactListViewData *data, Evas_Object *obj, void *event_info);
static void frame_list_call_clicked(struct ContactListViewData *data, Evas_Object *obj, void *event_info);
static void frame_list_options_clicked(struct ContactListViewData *data, Evas_Object *obj, void *event_info);
static void frame_list_message_clicked(struct ContactListViewData *data, Evas_Object *obj, void *event_info);
static void frame_list_edit_clicked(struct ContactListViewData *data, Evas_Object *obj, void *event_info);
static void frame_list_delete_clicked(struct ContactListViewData *data, Evas_Object *obj, void *event_info);
static void frame_list_contact_added(struct ContactListViewData *data);


struct ContactListViewData *contact_list_view_show(struct Window *win, GHashTable *options) {
    struct ContactListViewData *data = g_slice_alloc0(sizeof(struct ContactListViewData));
    data->win = win;

    window_frame_show(win, data, frame_list_show, frame_list_hide);
    window_show(win);
    return data;
}


void contact_list_view_hide(struct ContactListViewData *data) {
    g_slice_free(struct ContactListViewData, data);
    data = NULL;
}



/*
 * Frame "list"
 */

static void frame_list_show(struct ContactListViewData *data) {
    struct Window *win = data->win;

    window_layout_set(win, CONTACTS_FILE, "list");

    data->list = elm_my_contactlist_add(window_evas_object_get(data->win));
    window_swallow(data->win, "list", data->list);
    evas_object_show(data->list);

    data->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt1, "New");
    evas_object_smart_callback_add(data->bt1, "clicked", frame_list_new_clicked, data);
    window_swallow(win, "button_new", data->bt1);
    evas_object_show(data->bt1);

    data->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt2, "Call");
    evas_object_smart_callback_add(data->bt2, "clicked", frame_list_call_clicked, data);
    window_swallow(win, "button_call", data->bt2);
    evas_object_show(data->bt2);

    data->bt_options = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt_options, "Options");
    evas_object_smart_callback_add(data->bt_options, "clicked", frame_list_options_clicked, data);
    window_swallow(win, "button_options", data->bt_options);
    evas_object_show(data->bt_options);


    /* Options */
    data->hv = elm_hover_add(window_evas_object_get(win));
    elm_hover_parent_set(data->hv, window_evas_object_get(win));
    elm_hover_target_set(data->hv, data->bt_options);

    data->bx = elm_box_add(window_evas_object_get(win));
    elm_box_horizontal_set(data->bx, 0);
    elm_box_homogenous_set(data->bx, 1);
    evas_object_show(data->bx);

    data->bt_message = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt_message, "SMS");
    evas_object_size_hint_min_set(data->bt_message, 130, 80);
    evas_object_smart_callback_add(data->bt_message, "clicked", frame_list_message_clicked, data);
    evas_object_show(data->bt_message);
    elm_box_pack_end(data->bx, data->bt_message);

    data->bt_edit = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt_edit, "Edit");
    evas_object_size_hint_min_set(data->bt_edit, 130, 80);
    evas_object_smart_callback_add(data->bt_edit, "clicked", frame_list_edit_clicked, data);
    evas_object_show(data->bt_edit);
    elm_box_pack_end(data->bx, data->bt_edit);

    data->bt_delete = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt_delete, "Delete");
    evas_object_size_hint_min_set(data->bt_delete, 130, 80);
    evas_object_smart_callback_add(data->bt_delete, "clicked", frame_list_delete_clicked, data);
    evas_object_show(data->bt_delete);
    elm_box_pack_end(data->bx, data->bt_delete);

    elm_hover_content_set(data->hv, "top", data->bx);
}


static void frame_list_hide(struct ContactListViewData *data) {
    g_debug("frame_list_hide() called");

    evas_object_del(data->bt1);
    evas_object_del(data->bt2);
    evas_object_del(data->bt_options);
    evas_object_del(data->list);
}

static void frame_list_new_clicked(struct ContactListViewData *data, Evas_Object *obj, void *event_info) {
    g_debug("new");

    GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(options, "callback", frame_list_contact_added);
    g_hash_table_insert(options, "callback_data", data);

    struct Window *win = window_new("New Contact");
    window_init(win);
    window_view_show(win, options, contact_edit_view_show, contact_edit_view_hide);
}

static void frame_list_call_clicked(struct ContactListViewData *data, Evas_Object *obj, void *event_info) {
    GHashTable *properties = elm_my_contactlist_selected_row_get(data->list);
    if(properties != NULL) {
        ogsmd_call_initiate(g_hash_table_lookup(properties, "number"), "voice", NULL, NULL);
    }
}

static void frame_list_options_clicked(struct ContactListViewData *data, Evas_Object *obj, void *event_info) {
    evas_object_show(data->hv);
}

static void frame_list_message_clicked(struct ContactListViewData *data, Evas_Object *obj, void *event_info) {
    evas_object_hide(data->hv);

    g_debug("mssage");
    GHashTable *properties = elm_my_contactlist_selected_row_get(data->list);
    if(properties != NULL) {
        assert(g_hash_table_lookup(properties, "number") != NULL);

        GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
        g_hash_table_insert(options, "recipient", g_hash_table_lookup(properties, "number"));

        struct Window *win = window_new("Compose SMS");
        window_init(win);
        window_view_show(win, options, message_new_view_show, message_new_view_hide);
    }
}

static void frame_list_edit_clicked(struct ContactListViewData *data, Evas_Object *obj, void *event_info) {
    evas_object_hide(data->hv);

    GHashTable *properties = elm_my_contactlist_selected_row_get(data->list);
    if(properties != NULL) {
        GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
        g_hash_table_insert(options, "id", g_hash_table_lookup(properties, "id"));
        g_hash_table_insert(options, "name", g_hash_table_lookup(properties, "name"));
        g_hash_table_insert(options, "number", g_hash_table_lookup(properties, "number"));
        g_hash_table_insert(options, "callback", frame_list_contact_added);
        g_hash_table_insert(options, "callback_data", data);

        struct Window *win = window_new("Edit Contact");
        window_init(win);
        window_view_show(win, options, contact_edit_view_show, contact_edit_view_hide);
    }
}

static void frame_list_delete_clicked(struct ContactListViewData *data, Evas_Object *obj, void *event_info) {
    evas_object_show(data->hv);
}


static void frame_list_contact_added(struct ContactListViewData *data) {
    elm_my_contactlist_refresh(data->list);
}

