#include "views.h"
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>


struct ContactDeleteViewData {
    struct Window *win;
    int id;
    void (*callback)();
    void *callback_data;
    Evas_Object *bt_yes, *bt_no;
};

void contact_delete_yes_clicked(void *userdata, Evas_Object *obj, void *event_info);
void contact_delete_no_clicked(void *userdata, Evas_Object *obj, void *event_info);
static void delete_callback(GError *error, gpointer userdata);
static void delete_callback2(struct ContactDeleteViewData *data);

static void frame_delete_show(struct ContactDeleteViewData *data);
static void frame_delete_hide(struct ContactDeleteViewData *data);
static void frame_delete_no_clicked(void *userdata, Evas_Object *obj, void *event_info);
static void frame_delete_yes_clicked(void *userdata, Evas_Object *obj, void *event_info);

static void frame_deleting_show(struct ContactDeleteViewData *data);


struct ContactDeleteViewData *contact_delete_view_show(struct Window *win, GHashTable *options) {
    struct ContactDeleteViewData *data = g_slice_alloc0(sizeof(struct ContactDeleteViewData));
    data->win = win;

    if(options == NULL) {
        g_error("At least option[id] must be set.");
    } else {
        data->id = g_hash_table_lookup(options, "id");
        data->callback = g_hash_table_lookup(options, "delete_callback"); 
        data->callback_data = g_hash_table_lookup(options, "delete_callback_data"); 

        g_debug("Delete view with contact id = %d", data->id);
    }

    window_frame_show(win, data, frame_delete_show, frame_delete_hide);
    window_show(win);
    
    return data;
}

void contact_delete_view_hide(struct ContactDeleteViewData *data) {
    g_slice_free(struct ContactDeleteViewData, data);
}

static void delete_callback(GError *error, gpointer userdata) {
    struct ContactDeleteViewData *data = (struct ContactDeleteViewData *) userdata;
    async_trigger(delete_callback2, data);
}

static void delete_callback2(struct ContactDeleteViewData *data) {
    if(data->callback != NULL) {
        data->callback(data->callback_data);
    }

    window_destroy(data->win, NULL);    
}



/*
 * Frame "delete"
 */

static void frame_delete_show(struct ContactDeleteViewData *data) {
    struct Window *win = data->win;
    window_layout_set(win, CONTACTS_FILE, "delete");

    data->bt_yes = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt_yes, "Yes");
    evas_object_smart_callback_add(data->bt_yes, "clicked", frame_delete_yes_clicked, data);
    window_swallow(win, "button_yes", data->bt_yes);
    evas_object_show(data->bt_yes);

    data->bt_no = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt_no, "No");
    evas_object_smart_callback_add(data->bt_no, "clicked", frame_delete_no_clicked, data);
    window_swallow(win, "button_no", data->bt_no);
    evas_object_show(data->bt_no);
}

static void frame_delete_hide(struct ContactDeleteViewData *data) {
    evas_object_del(data->bt_yes);
    evas_object_del(data->bt_no);
}


static void frame_delete_no_clicked(void *userdata, Evas_Object *obj, void *event_info) {
    struct ContactDeleteViewData *data = (struct ContactDeleteViewData *) userdata;
    window_destroy(data->win, NULL);
}

static void frame_delete_yes_clicked(void *userdata, Evas_Object *obj, void *event_info) {
    struct ContactDeleteViewData *data = (struct ContactDeleteViewData *) userdata;
    window_frame_show(data->win, data, frame_deleting_show, NULL);

    ogsmd_sim_delete_entry("contacts", data->id, delete_callback, data);
}


/*
 * Frame "deleting"
 */

static void frame_deleting_show(struct ContactDeleteViewData *data) {
    struct Window *win = data->win;
    window_layout_set(win, CONTACTS_FILE, "deleting");
}

