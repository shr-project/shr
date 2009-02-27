#include "views.h"
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>


struct MessageDeleteViewData {
    struct Window *win;
    int id;
    void (*callback)();
    void *callback_data;
    Evas_Object *bt_yes, *bt_no;
};

void message_delete_yes_clicked(void *userdata, Evas_Object *obj, void *event_info);
void message_delete_no_clicked(void *userdata, Evas_Object *obj, void *event_info);
static void delete_callback(GError *error, gpointer userdata);
static void delete_callback2(struct MessageDeleteViewData *data);

static void frame_delete_show(struct MessageDeleteViewData *data);
static void frame_delete_hide(struct MessageDeleteViewData *data);
static void frame_delete_no_clicked(void *userdata, Evas_Object *obj, void *event_info);
static void frame_delete_yes_clicked(void *userdata, Evas_Object *obj, void *event_info);

static void frame_deleting_show(struct MessageDeleteViewData *data);


struct MessageDeleteViewData *message_delete_view_show(struct Window *win, GHashTable *options) {
    struct MessageDeleteViewData *data = g_slice_alloc0(sizeof(struct MessageDeleteViewData));
    data->win = win;

    if(options == NULL) {
        g_error("At least option[id] must be set.");
    } else {
        data->id = g_hash_table_lookup(options, "id");
        data->callback = g_hash_table_lookup(options, "delete_callback"); 
        data->callback_data = g_hash_table_lookup(options, "delete_callback_data"); 

        g_debug("Delete view with message id = %d", data->id);
    }

    window_frame_show(win, data, frame_delete_show, frame_delete_hide);
    window_show(win);
    
    return data;
}

void message_delete_view_hide(struct MessageDeleteViewData *data) {
    g_slice_free(struct MessageDeleteViewData, data);
}

static void delete_callback(GError *error, gpointer userdata) {
    struct MessageDeleteViewData *data = (struct MessageDeleteViewData *) userdata;
    async_trigger(delete_callback2, data);
}

static void delete_callback2(struct MessageDeleteViewData *data) {
    if(data->callback != NULL) {
        data->callback(data->callback_data);
    }

    window_destroy(data->win, NULL);    
}



/*
 * Frame "delete"
 */

static void frame_delete_show(struct MessageDeleteViewData *data) {
    struct Window *win = data->win;
    window_layout_set(win, MESSAGE_FILE, "delete");

    window_text_set(win, "info", D_("Do you really want to delete it?"));

    data->bt_yes = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt_yes, D_("Yes"));
    evas_object_smart_callback_add(data->bt_yes, "clicked", frame_delete_yes_clicked, data);
    window_swallow(win, "button_yes", data->bt_yes);
    evas_object_show(data->bt_yes);

    data->bt_no = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt_no, D_("No"));
    evas_object_smart_callback_add(data->bt_no, "clicked", frame_delete_no_clicked, data);
    window_swallow(win, "button_no", data->bt_no);
    evas_object_show(data->bt_no);
}

static void frame_delete_hide(struct MessageDeleteViewData *data) {
    evas_object_del(data->bt_yes);
    evas_object_del(data->bt_no);
}


static void frame_delete_no_clicked(void *userdata, Evas_Object *obj, void *event_info) {
    struct MessageDeleteViewData *data = (struct MessageDeleteViewData *) userdata;
    window_destroy(data->win, NULL);
}

static void frame_delete_yes_clicked(void *userdata, Evas_Object *obj, void *event_info) {
    struct MessageDeleteViewData *data = (struct MessageDeleteViewData *) userdata;
    window_frame_show(data->win, data, frame_deleting_show, NULL);

    ogsmd_sim_delete_message(data->id, delete_callback, data);
}


/*
 * Frame "deleting"
 */

static void frame_deleting_show(struct MessageDeleteViewData *data) {
    struct Window *win = data->win;
    window_layout_set(win, MESSAGE_FILE, "deleting");
    window_text_set(win, "text", D_("Deleting.."));
}

