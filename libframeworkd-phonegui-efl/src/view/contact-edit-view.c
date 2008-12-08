#include "views.h"


struct ContactEditViewData {
    struct Window *win;
    Evas_Object *bt1, *bt2;
    Evas_Object *entry_name, *entry_number;
    Evas_Object *sc_name, *sc_number;

    int id;
    char *name, *number;
    void (*callback)();
    void *callback_data;
    gboolean saved;
    
    GPtrArray *contacts;
    int free_entry_index;

    struct ContactEditViewData **data_pointer;
};


static void get_phonebook_info_callback(GError *error, GHashTable *info, gpointer userdata);
int calculate_free_entry_index(int min, int max, GPtrArray *entries, struct ContactEditViewData *data);
static void retrieve_callback(GError*error, GPtrArray*messages, gpointer userdata);

static void frame_edit_show(struct ContactEditViewData *data);
static void frame_edit_hide(struct ContactEditViewData *data);
static void frame_edit_save_clicked(struct ContactEditViewData *data, Evas_Object *obj, void *event_info);
static void frame_edit_close_clicked(struct ContactEditViewData *data, Evas_Object *obj, void *event_info);
static char* frame_edit_name_get(struct ContactEditViewData *data);
static char* frame_edit_number_get(struct ContactEditViewData *data);
static void frame_edit_save_callback(GError *error, struct ContactEditViewData *data);
static void frame_edit_save_callback2(struct ContactEditViewData *data);

static void frame_close_show(struct ContactEditViewData *data);
static void frame_close_hide(struct ContactEditViewData *data);
static void frame_close_yes_clicked(struct ContactEditViewData *data, Evas_Object *obj, void *event_info);
static void frame_close_no_clicked(struct ContactEditViewData *data, Evas_Object *obj, void *event_info);

static void frame_loading_show(struct ContactEditViewData *data);
static void frame_loading_hide(struct ContactEditViewData *data);



struct ContactEditViewData *contact_edit_view_show(struct Window *win, GHashTable *options) {
    struct ContactEditViewData *data = g_slice_alloc0(sizeof(struct ContactEditViewData));
    data->win = win;
    data->id = -1;
    data->name = strdup("");
    data->number = strdup("");
    data->saved = FALSE;
    data->callback = NULL;
    data->callback_data = NULL;
    data->data_pointer = malloc(sizeof(struct ContactEditViewData **));
    *(data->data_pointer) = data; // used for async dbus calls

    if(options != NULL) {
        if(g_hash_table_lookup(options, "name")) {
            data->name = strdup(g_hash_table_lookup(options, "name"));

            // ID should only be set if name is specified
            data->id = g_hash_table_lookup(options, "id");
        }

        if(g_hash_table_lookup(options, "number")) {
            data->number = strdup(g_hash_table_lookup(options, "number"));
        }

        data->callback = g_hash_table_lookup(options, "change_callback"); 
        data->callback_data = g_hash_table_lookup(options, "change_callback_data"); 
    }

    if(data->id == -1) {
        ogsmd_sim_retrieve_phonebook("contacts", retrieve_callback, (void *) data->data_pointer);
    }

    window_frame_show(win, data, frame_edit_show, frame_edit_hide);
    window_show(win);
    return data;
}


void contact_edit_view_hide(struct ContactEditViewData *data) {
    if(data->saved && data->callback != NULL) {
        data->callback(data->callback_data);
    }

    *(data->data_pointer) = NULL;
    g_slice_free(struct ContactEditViewData, data);
}



static void get_phonebook_info_callback(GError *error, GHashTable *info, gpointer userdata) {
    struct ContactEditViewData **data = (struct ContactEditViewData **) userdata;
    if(*data == NULL) {
        free(data);
    } else {
        int min = g_value_get_int(g_hash_table_lookup(info, "min_index"));
        int max = g_value_get_int(g_hash_table_lookup(info, "max_index"));
        (*data)->free_entry_index = calculate_free_entry_index(min, max, (*data)->contacts, *data);
        g_debug("index: %d", (*data)->free_entry_index);
    }
}


void add_entry_index(GValueArray *entry, int* indizes) {
    int i = 0;
    while(indizes[i] != -1)
        i++;

    int id = g_value_get_int(g_value_array_get_nth(entry, 0));
    indizes[i] = id;
}


int calculate_free_entry_index(int min, int max, GPtrArray *entries, struct ContactEditViewData *data) {
    int *indizes = g_slice_alloc(sizeof(int) * entries->len);
    memset(indizes, -1, sizeof(int) * entries->len);
    g_ptr_array_foreach(entries, add_entry_index, indizes);

    // TODO: Optimize the loops
    int ret = -1;
    int i, j;
    g_debug("length: %d", data->contacts->len);

    for(i = min ; i <= max && ret == -1 ; i++) {
        gboolean found = FALSE;
        for(j = 0 ; j < data->contacts->len && found == FALSE ; j++) {
            if(i == indizes[j]) {
                found = TRUE;
            }
        }
        if(found == FALSE) {
            ret = i;
            g_debug("Found free entry: %d", i);
        }
    }

    return ret;
}


static void retrieve_callback(GError*error, GPtrArray*contacts, gpointer userdata) {
    struct ContactEditViewData **data = (struct ContactEditViewData **) userdata;
    if(*data == NULL) {
        free(data);
    } else {
        (*data)->contacts = contacts;
        ogsmd_sim_get_phonebook_info("contacts", get_phonebook_info_callback, data);
    }
}



/*
 * View "edit"
 */

static void frame_edit_show(struct ContactEditViewData *data) {
    struct Window *win = data->win;

    window_kbd_show(win, KEYBOARD_ALPHA);

    window_layout_set(win, CONTACTS_FILE, "edit");
    if(data->id == -1)
        window_text_set(win, "title", "New Contact");
    else
        window_text_set(win, "title", "Edit Contact");

    data->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt1, "Close");
    evas_object_smart_callback_add(data->bt1, "clicked", frame_edit_close_clicked, data);
    window_swallow(win, "button_back", data->bt1);
    evas_object_show(data->bt1);

    data->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt2, "Save");
    evas_object_smart_callback_add(data->bt2, "clicked", frame_edit_save_clicked, data);
    window_swallow(win, "button_save", data->bt2);
    evas_object_show(data->bt2);


    data->entry_name = elm_entry_add(window_evas_object_get(win));
    elm_entry_entry_set(data->entry_name, data->name);
    evas_object_show(data->entry_name);
    elm_widget_focus_set(data->entry_name, 1);

    data->sc_name = elm_scroller_add(window_evas_object_get(win));
    elm_scroller_content_set(data->sc_name, data->entry_name);
    window_swallow(win, "entry_name", data->sc_name);
    evas_object_show(data->sc_name);


    data->entry_number = elm_entry_add(window_evas_object_get(win));
    elm_entry_entry_set(data->entry_number, data->number);
    evas_object_show(data->entry_number);

    data->sc_number = elm_scroller_add(window_evas_object_get(win));
    elm_scroller_content_set(data->sc_number, data->entry_number);
    window_swallow(win, "entry_number", data->sc_number);
    evas_object_show(data->sc_number);
}

static void frame_edit_hide(struct ContactEditViewData *data) {
    struct Window *win = data->win;

    free(data->name);
    free(data->number);
    data->name = frame_edit_name_get(data);
    data->number = frame_edit_number_get(data);

    evas_object_del(data->bt1);
    evas_object_del(data->bt2);
    evas_object_del(data->entry_name);
    evas_object_del(data->entry_number);
    evas_object_del(data->sc_name);
    evas_object_del(data->sc_number);

    window_kbd_hide(win);
}

static void frame_edit_save_clicked(struct ContactEditViewData *data, Evas_Object *obj, void *event_info) {
    if(!strlen(frame_edit_name_get(data)) || !strlen(frame_edit_number_get(data)) || !string_is_number(frame_edit_number_get(data)))
        return;

    window_frame_show(data->win, data, frame_loading_show, frame_loading_hide);

    if(data->id == -1) {
        ogsmd_sim_store_entry(
            "contacts",
            data->free_entry_index,
            data->name,
            data->number,
            frame_edit_save_callback,
            data
        );
    } else {
        ogsmd_sim_store_entry(
            "contacts",
            data->id,
            data->name,
            data->number,
            frame_edit_save_callback,
            data
        );

    }

    data->saved = TRUE;

    // TODO: Free name and number
}

static void frame_edit_save_callback(GError *error, struct ContactEditViewData *data) {
    async_trigger(frame_edit_save_callback2, data);
}

static void frame_edit_save_callback2(struct ContactEditViewData *data) {
    window_destroy(data->win, NULL);
}

static void frame_edit_close_clicked(struct ContactEditViewData *data, Evas_Object *obj, void *event_info) {
    window_frame_show(data->win, data, frame_close_show, frame_close_hide);
}

static char* frame_edit_name_get(struct ContactEditViewData *data) {
    char *name = g_strstrip(elm_entry_entry_get(data->entry_name));
    string_strip_html(name);
    return name;
}

static char* frame_edit_number_get(struct ContactEditViewData *data) {
    char *number = g_strstrip(elm_entry_entry_get(data->entry_number));
    string_strip_html(number);
    return number;
}

/*void contacts_sim_full_show(struct ContactEditViewData *data) {
    window_layout_set(win, UI_FILE, "dialog");
    window_text_set(win, "content", "Your storage is full. Before adding new contacts, you have to delete some old ones.");

    data->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt1, "Close");
    evas_object_smart_callback_add(data->bt1, "clicked", contacts_button_close_clicked, win);
    window_swallow(win, "button_close", data->bt1);
    evas_object_show(data->bt1);
}

void contacts_sim_full_hide(struct ContactEditViewData *data) {
    window_unswallow(win, data->bt1);
    evas_object_del(data->bt1);
}*/




/*
 * Frame "close" (confirmation)
 */

static void frame_close_show(struct ContactEditViewData *data) {
    struct Window *win = data->win;

    window_layout_set(win, DIALOG_FILE, "close");
    window_text_set(win, "information", "Do you really want to quit?");

    data->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt1, "Yes");
    evas_object_smart_callback_add(data->bt1, "clicked", frame_close_yes_clicked, data);
    window_swallow(win, "button_yes", data->bt1);
    evas_object_show(data->bt1);

    data->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt2, "No");
    evas_object_smart_callback_add(data->bt2, "clicked", frame_close_no_clicked, data);
    window_swallow(win, "button_no", data->bt2);
    evas_object_show(data->bt2);
}

static void frame_close_hide(struct ContactEditViewData *data) {
    evas_object_del(data->bt1);
    evas_object_del(data->bt2);
}

static void frame_close_yes_clicked(struct ContactEditViewData *data, Evas_Object *obj, void *event_info) {
    window_destroy(data->win, NULL);
}

static void frame_close_no_clicked(struct ContactEditViewData *data, Evas_Object *obj, void *event_info) {
    window_frame_show(data->win, data, frame_edit_show, frame_edit_hide);
}


/*
 * Frame "loading"
 */

static void frame_loading_show(struct ContactEditViewData *data) {
    window_layout_set(data->win, CONTACTS_FILE, "loading");
}

static void frame_loading_hide(struct ContactEditViewData *data) {

}

