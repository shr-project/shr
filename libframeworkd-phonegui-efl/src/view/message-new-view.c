#include "views.h"

enum MessageNewModes {
    MODE_CONTENT,
    MODE_RECIPIENT,
    MODE_RECIPIENT_NUMBER,
    MODE_RECIPIENT_CONTACT,
    MODE_CLOSE
};

struct MessageNewViewData {
    struct Window *win;
    int mode;
    char *content;
    Evas_Object *bb, *entry, *bt1, *bt2, *bt3, *bt4, *bt5, *hv, *bx, *hbt1, *hbt2, *hbt3, *sc;
    Evas_Object *list_contacts;
    Evas_Object *information;

    GPtrArray *recipients;
    Evas_Object *list_recipients, *container_recipients_eo;
    Etk_Widget *container_recipients, *tree_recipients;
    Etk_Tree_Col *col1_recipients;

    int messages_sent;
};


static void message_send_callback(GError *error, int transaction_index, struct MessageNewViewData *data);

static void frame_content_show(struct MessageNewViewData *data);
static void frame_content_hide(struct MessageNewViewData *data);
static void frame_content_close_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info);
static void frame_content_continue_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info);
static void frame_content_content_changed(struct MessageNewViewData *data, Evas_Object *obj, void *event_info);

static void frame_recipient_show(struct MessageNewViewData *data);
static void frame_recipient_hide(struct MessageNewViewData *data);
static void frame_recipient_back_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info);
static void frame_recipient_contact_add_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info);
static void frame_recipient_number_add_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info);
static void frame_recipient_delete_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info);
static void frame_recipient_continue_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info);
static void frame_recipient_process_recipient(gpointer _properties, gpointer _data);
static void frame_recipient_send_callback(GError *error, int transaction_index, const char *timestamp, struct MessageNewViewData *data);
static void frame_recipient_send_callback2(struct MessageNewViewData *data);

static void frame_contact_add_show(struct MessageNewViewData *data);
static void frame_contact_add_hide(struct MessageNewViewData *data);
static void frame_contact_add_back_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info);
static void frame_contact_add_add_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info);

static void frame_number_add_show(struct MessageNewViewData *data);
static void frame_number_add_hide(struct MessageNewViewData *data);
static void frame_number_add_add_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info);
static void frame_number_add_back_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info);

static void frame_close_show(struct MessageNewViewData *data);
static void frame_close_hide(struct MessageNewViewData *data);
static void frame_close_yes_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info);
static void frame_close_no_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info);

static void frame_sending_show(struct MessageNewViewData *data);
static void frame_sending_hide(struct MessageNewViewData *data);


struct MessageNewViewData *message_new_view_show(struct Window *win, GHashTable *options) {
    struct MessageNewViewData *data = g_slice_alloc0(sizeof(struct MessageNewViewData));
    data->win = win;
    data->mode = MODE_CONTENT;
    data->content = NULL;
    data->recipients = g_ptr_array_new();
    data->messages_sent = 0;

    if(options != NULL) {
        char *recipient = g_hash_table_lookup(options, "recipient");
        if(recipient != NULL) {
            GHashTable *properties = g_hash_table_new(g_str_hash, g_str_equal);
            g_hash_table_insert(properties, strdup("name"), strdup("Number"));
            g_hash_table_insert(properties, strdup("number"), strdup(recipient));
            g_ptr_array_add(data->recipients, properties);
        }
    }


    window_frame_show(win, data, frame_content_show, frame_content_hide);
    window_show(win);

    return data;
}

void message_new_view_hide(struct MessageNewViewData *data) {
    //g_slice_free(struct MessageNewViewData, data);
}

static void message_send_callback(GError *error, int transaction_index, struct MessageNewViewData *data) {
    g_debug("message_send_callback()");
}




/*
 * Frame "content"
 */

static void frame_content_show(struct MessageNewViewData *data) {
    struct Window *win = data->win;

    elm_layout_file_set(win->layout, MESSAGE_FILE, "content_edit");

    data->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt1, D_("Close"));
    evas_object_smart_callback_add(data->bt1, "clicked", frame_content_close_clicked, data);
    window_swallow(win, "button_close", data->bt1);
    evas_object_show(data->bt1);

    data->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt2, D_("Continue"));
    evas_object_smart_callback_add(data->bt2, "clicked", frame_content_continue_clicked, data);
    window_swallow(win, "button_continue", data->bt2);
    evas_object_show(data->bt2);

    data->sc = elm_scroller_add(window_evas_object_get(win));
    elm_scroller_policy_set(data->sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_ON);
    data->entry = elm_entry_add(window_evas_object_get(win));
    evas_object_size_hint_weight_set(data->entry, 1.0, 1.0);
    evas_object_size_hint_align_set(data->entry, -1.0, -1.0);
    evas_object_smart_callback_add(data->entry, "changed", frame_content_content_changed, data);
    if(data->content != NULL) {
        elm_entry_entry_set(data->entry, data->content);
    }
    elm_scroller_content_set(data->sc, data->entry);
    evas_object_show(data->entry);

    window_swallow(win, "entry", data->sc);
    evas_object_show(data->sc);

    elm_object_focus(data->entry);
}

static void frame_content_hide(struct MessageNewViewData *data) {
    struct Window *win = data->win;

    // Save content
    data->content = g_strstrip(elm_entry_markup_to_utf8(elm_entry_entry_get(data->entry)));

    // Free objects
    evas_object_del(data->bt1);
    evas_object_del(data->bt2);
    evas_object_del(data->entry);
    evas_object_del(data->sc);

    window_kbd_hide(win);
}

static void frame_content_close_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info) {
    window_frame_show(data->win, data, frame_close_show, frame_close_hide);
}

static void frame_content_continue_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info) {
    char *content = g_strstrip(elm_entry_markup_to_utf8(elm_entry_entry_get(data->entry)));
    if(strlen(content)==0) {
        //TODO: display notify about sending blank message
    }
    data->mode = MODE_RECIPIENT;
    window_frame_show(data->win, data, frame_recipient_show, frame_recipient_hide);
    free(content);
}

/* important notice: not sure how sms split is handled,
 * iirc at a certain point msg lens are not the same as before,
 * not 100% sure. should verify
 */
static void
frame_content_content_changed(struct MessageNewViewData *data, Evas_Object *obj, void *event_info)
{

    char *content = g_strstrip(strdup(elm_entry_entry_get(data->entry)));
    int limit; /* the limit of the sms */
    int len; /* the number of characters in the sms */
    int size; /* the "memory" size of the sms */
    char text[64];

    string_strip_html(content);

    //g_debug("content: %s", content);

    
    for (size = 0, len = 0; content[size]; len++) 
	evas_common_font_utf8_get_next((unsigned char *)content, &size);
    
    /* if it includes chars that can't be represented
     * with 7bit encoding, this sms will be sent as ucs-2 treat
     * it this way! */
    if (size > len) 
	limit = 70; /* ucs-2 number of chars limit */
    else 
	limit = 160; /* regular number of chars limit */
    

    /*FIXME: BAD BAD BAD! will cause an overflow when using a long translation!!! */
    sprintf(text, D_("%d characters left [%d]"), limit - (len % limit), (len / limit) + 1);
    window_text_set(data->win, "characters_left", text);
    free(content);
}



/*
 * Frame "recipient"
 */

static void frame_recipient_show(struct MessageNewViewData *data) {
    struct Window *win = data->win;

    window_layout_set(win, MESSAGE_FILE, "recipient_edit");

    window_text_set(win, "title", D_("Define Recipients"));

    data->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt1, D_("Back"));
    evas_object_smart_callback_add(data->bt1, "clicked", frame_recipient_back_clicked, data);
    window_swallow(win, "button_back", data->bt1);
    evas_object_show(data->bt1);

    data->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt2, D_("Send"));
    evas_object_smart_callback_add(data->bt2, "clicked", frame_recipient_continue_clicked, data);
    window_swallow(win, "button_continue", data->bt2);
    evas_object_show(data->bt2);

    data->bt3 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt3, D_("Add Contact"));
    evas_object_smart_callback_add(data->bt3, "clicked", frame_recipient_contact_add_clicked, data);
    window_swallow(win, "button_contact_add", data->bt3);
    evas_object_show(data->bt3);

    data->bt4 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt4, D_("Add Number"));
    evas_object_smart_callback_add(data->bt4, "clicked", frame_recipient_number_add_clicked, data);
    window_swallow(win, "button_number_add", data->bt4);
    evas_object_show(data->bt4);

    data->bt5 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt5, D_("Remove"));
    evas_object_smart_callback_add(data->bt5, "clicked", frame_recipient_delete_clicked, data);
    window_swallow(win, "button_delete", data->bt5);
    evas_object_show(data->bt5);

    data->tree_recipients = etk_tree_new();
    etk_tree_rows_height_set(ETK_TREE(data->tree_recipients), 80);
    etk_tree_mode_set(ETK_TREE(data->tree_recipients), ETK_TREE_MODE_LIST);
    etk_tree_headers_visible_set(ETK_TREE(data->tree_recipients), ETK_FALSE);
    etk_tree_multiple_select_set(ETK_TREE(data->tree_recipients), ETK_FALSE);

    data->col1_recipients = etk_tree_col_new(ETK_TREE(data->tree_recipients), "Title", 300, 0.0);
    etk_tree_col_model_add(data->col1_recipients, etk_tree_model_edje_new(CONTACTS_FILE, "row"));
    etk_tree_build(ETK_TREE(data->tree_recipients));

    Etk_Scrolled_View *scrolled_view = etk_tree_scrolled_view_get(ETK_TREE(data->tree_recipients));
    etk_scrolled_view_dragable_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_TRUE);
    etk_scrolled_view_drag_bouncy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_FALSE);
    etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_POLICY_HIDE, ETK_POLICY_HIDE);

    data->container_recipients = etk_embed_new(evas_object_evas_get(window_evas_object_get(win)));
    etk_container_add(ETK_CONTAINER(data->container_recipients), data->tree_recipients);
    etk_widget_show_all(data->container_recipients);
    data->container_recipients_eo = etk_embed_object_get(ETK_EMBED(data->container_recipients));

    window_swallow(win, "list", data->container_recipients_eo);

    g_ptr_array_foreach(data->recipients, frame_recipient_process_recipient, data);
}

static void frame_recipient_hide(struct MessageNewViewData *data) {
    g_debug("frame_recipient_hide()");

    struct Window *win = data->win;

    evas_object_del(data->bt1);
    evas_object_del(data->bt2);
    evas_object_del(data->bt3);
    evas_object_del(data->bt4);
    evas_object_del(data->bt5);
    evas_object_del(data->container_recipients_eo);
}

static void frame_recipient_back_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info) {
    data->mode = MODE_CONTENT;
    window_frame_show(data->win, data, frame_content_show, frame_content_hide);
}

static void frame_recipient_contact_add_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info) {
    data->mode = MODE_RECIPIENT_CONTACT;
    window_frame_show(data->win, data, frame_contact_add_show, frame_contact_add_hide);
}

static void frame_recipient_number_add_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info) {
    data->mode = MODE_RECIPIENT_NUMBER;
    window_frame_show(data->win, data, frame_number_add_show, frame_number_add_hide);
}

static void frame_recipient_delete_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info) {
    Etk_Tree_Row *row = etk_tree_selected_row_get(data->tree_recipients);
    if(row != NULL) {
        GHashTable *parameters = etk_tree_row_data_get(row);
        g_ptr_array_remove(data->recipients, parameters);
        etk_tree_clear(data->tree_recipients);
        g_ptr_array_foreach(data->recipients, frame_recipient_process_recipient, data);
    }
}

static void frame_recipient_continue_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info) {
    if(data->recipients->len) {
        window_frame_show(data->win, data, frame_sending_show, frame_sending_hide);

        int i;
        for (i = 0 ; i < data->recipients->len; i++) {
            GHashTable *properties = (GHashTable *) g_ptr_array_index(data->recipients, i);
            char *number = (char *) g_hash_table_lookup(properties, "number");
            assert(number != NULL);

            GHashTable *options = g_hash_table_new(NULL, NULL);
            // TODO: Remove strdup
            ogsmd_sms_send_message(strdup(number), strdup(data->content), options, frame_recipient_send_callback, data);
            frame_recipient_send_callback(NULL, 1, "", data);
        }
    }
}

static void frame_recipient_send_callback(GError *error, int transaction_index, const char *timestamp, struct MessageNewViewData *data) {
    data->messages_sent++;
    if(data->messages_sent == data->recipients->len) {
        sleep(1);
        async_trigger(frame_recipient_send_callback2, data);
    }
}

static void frame_recipient_send_callback2(struct MessageNewViewData *data) {
    window_destroy(data->win, NULL);
}

static void frame_recipient_process_recipient(gpointer _properties, gpointer _data) {
    GHashTable *properties = (GHashTable *)_properties;
    struct MessageNewViewData *data = (struct MessageNewViewData *)_data;

    Etk_Tree_Row *row = etk_tree_row_append(ETK_TREE(data->tree_recipients), NULL, data->col1_recipients, properties, NULL);
    etk_tree_row_data_set(row, properties);
}



/*
 * Frame "contact_add"
 */

static void frame_contact_add_show(struct MessageNewViewData *data) {
    struct Window *win = data->win;
    window_layout_set(win, MESSAGE_FILE, "recipient_contact_add");

    window_text_set(win, "title", D_("Add a Contact"));

    data->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt1, D_("Back"));
    evas_object_smart_callback_add(data->bt1, "clicked", frame_contact_add_back_clicked, data);
    window_swallow(win, "button_back", data->bt1);
    evas_object_show(data->bt1);

    data->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt2, D_("Add"));
    evas_object_smart_callback_add(data->bt2, "clicked", frame_contact_add_add_clicked, data);
    window_swallow(win, "button_add", data->bt2);
    evas_object_show(data->bt2);

    data->list_contacts = elm_my_contactlist_add(window_evas_object_get(win));
    window_swallow(win, "list", data->list_contacts);
    evas_object_show(data->list_contacts);
}

static void frame_contact_add_hide(struct MessageNewViewData *data) {
    evas_object_del(data->bt1);
    evas_object_del(data->bt2);
    evas_object_del(data->list_contacts);
}

static void frame_contact_add_back_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info) {
    g_debug("frame_contact_add_back_clicked()");
    data->mode = MODE_RECIPIENT;
    window_frame_show(data->win, data, frame_recipient_show, frame_recipient_hide);
}

static void frame_contact_add_add_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info) {
    g_debug("frame_contact_add_add_clicked()");
    GHashTable *properties = elm_my_contactlist_selected_row_get(data->list_contacts);
    if(properties != NULL) {
        g_ptr_array_add(data->recipients, properties);
        data->mode = MODE_RECIPIENT;
        window_frame_show(data->win, data, frame_recipient_show, frame_recipient_hide);
    }
}



/*
 * Frame "number_add"
 */

static void frame_number_add_show(struct MessageNewViewData *data) {
    struct Window *win = data->win;

    window_layout_set(win, MESSAGE_FILE, "recipient_number_add");

    window_text_set(win, "title", D_("Add a Number"));

    data->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt1, D_("Back"));
    evas_object_smart_callback_add(data->bt1, "clicked", frame_number_add_back_clicked, data);
    window_swallow(win, "button_back", data->bt1);
    evas_object_show(data->bt1);

    data->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt2, D_("Add"));
    evas_object_smart_callback_add(data->bt2, "clicked", frame_number_add_add_clicked, data);
    window_swallow(win, "button_add", data->bt2);
    evas_object_show(data->bt2);

    data->entry = elm_entry_add(window_evas_object_get(win));
    evas_object_show(data->entry);
    elm_widget_focus_set(data->entry, 1);

    data->sc = elm_scroller_add(window_evas_object_get(win));
    elm_scroller_content_set(data->sc, data->entry);
    window_swallow(win, "entry", data->sc);
    evas_object_show(data->sc);

    window_kbd_show(win, KEYBOARD_NUMERIC);
}

static void frame_number_add_hide(struct MessageNewViewData *data) {
    evas_object_del(data->bt1);
    evas_object_del(data->bt2);
    evas_object_del(data->sc);
    evas_object_del(data->entry);

    window_kbd_hide(data->win);
}

static void frame_number_add_back_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info) {
    data->mode = MODE_RECIPIENT;
    window_frame_show(data->win, data, frame_recipient_show, frame_recipient_hide);
}

static void frame_number_add_add_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info) {
    char *number = strdup(elm_entry_entry_get(data->entry));
    string_strip_html(number);

    if(string_is_number(number)) {
        GHashTable *properties = g_hash_table_new(g_str_hash, g_str_equal);
        g_hash_table_insert(properties, strdup("name"), strdup("Number"));
        g_hash_table_insert(properties, strdup("number"), strdup(number));
        g_ptr_array_add(data->recipients, properties);

        data->mode = MODE_RECIPIENT;
        window_frame_show(data->win, data, frame_recipient_show, frame_recipient_hide);
    }

    free(number);
}


/*
 * Frame "close" (confirmation)
 */

static void frame_close_show(struct MessageNewViewData *data) {
    struct Window *win = data->win;

    window_layout_set(win, DIALOG_FILE, "close");

    data->information = elm_label_add( window_evas_object_get(win) );
    elm_label_label_set( data->information,  D_("Do you really want to quit?"));
    window_swallow(win, "information", data->information);
    evas_object_show(data->information);

    data->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt1, D_("Yes"));
    evas_object_smart_callback_add(data->bt1, "clicked", frame_close_yes_clicked, data);
    window_swallow(win, "button_yes", data->bt1);
    evas_object_show(data->bt1);

    data->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt2, D_("No"));
    evas_object_smart_callback_add(data->bt2, "clicked", frame_close_no_clicked, data);
    window_swallow(win, "button_no", data->bt2);
    evas_object_show(data->bt2);
}

static void frame_close_hide(struct MessageNewViewData *data) {
    evas_object_del(data->bt1);
    evas_object_del(data->bt2);
    evas_object_del(data->information);
}

static void frame_close_yes_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info) {
    window_destroy(data->win, NULL);
}

static void frame_close_no_clicked(struct MessageNewViewData *data, Evas_Object *obj, void *event_info) {
    window_frame_show(data->win, data, frame_content_show, frame_content_hide);
}



/*
 * Frame "sending"
 */

static void frame_sending_show(struct MessageNewViewData *data) {
    window_layout_set(data->win, MESSAGE_FILE, "sending");
    window_text_set(data->win, "text", D_("Sending.."));
}

static void frame_sending_hide(struct MessageNewViewData *data) {

}


