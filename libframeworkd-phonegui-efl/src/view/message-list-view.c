#include "views.h"
#include "Etk.h"

struct MessageListViewData {
    struct Window *win;

    Etk_Range *scroll_range;
    Etk_Widget *container, *tree_folders, *tree_messages;
    Etk_Tree_Row *selected_row;
    Etk_Tree_Col *col1;

    Etk_Entry *number_entry;
    Etk_Widget *container_number;

    char *messages_category;

    GPtrArray *tmp_messages;
    GValueArray *tmp_message;

    char *recipient_number;
    char *new_message_content;

    Evas_Object *content_entry, *bt1, *bt2, *bt3, *hv, *bx, *hbt1, *hbt2, *hbt3;
};



static void my_hover_bt_1(void *data, Evas_Object *obj, void *event_info);
static void message_list_view_back_clicked(struct MessageListViewData *data, Evas_Object *obj, void *event_info);
static void message_list_view_show_clicked(struct MessageListViewData *data, Evas_Object *obj, void *event_info);
static void message_list_view_delete_clicked(struct MessageListViewData *data, Evas_Object *obj, void *event_info);
static void message_list_view_answer_clicked(struct MessageListViewData *data, Evas_Object *obj, void *event_info);


struct MessageListViewData *message_list_view_show(struct Window *win, GHashTable *options) {
    g_debug("messages_list_view_show()");

    struct MessageListViewData *data = g_slice_alloc0(sizeof(struct MessageListViewData));
    data->win = win;


    window_layout_set(win, MESSAGE_FILE, "list");
    window_text_set(win, "title", "Inbox"); 

    data->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt1, "Back");
    evas_object_smart_callback_add(data->bt1, "clicked", message_list_view_back_clicked, data);
    window_swallow(win, "button_back", data->bt1);
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
    elm_button_label_set(data->hbt1, "Show");
    evas_object_size_hint_min_set(data->hbt1, 130, 80);
    evas_object_smart_callback_add(data->hbt1, "clicked", message_list_view_show_clicked, data);
    evas_object_show(data->hbt1);
    elm_box_pack_end(data->bx, data->hbt1);

    data->hbt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->hbt2, "Answer");
    evas_object_size_hint_min_set(data->hbt2, 130, 80);
    evas_object_smart_callback_add(data->hbt2, "clicked", message_list_view_answer_clicked, data);
    evas_object_show(data->hbt2);
    elm_box_pack_end(data->bx, data->hbt2);

    data->hbt3 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->hbt3, "Delete");
    evas_object_size_hint_min_set(data->hbt3, 130, 80);
    evas_object_smart_callback_add(data->hbt3, "clicked", message_list_view_delete_clicked, data);
    evas_object_show(data->hbt3);
    elm_box_pack_end(data->bx, data->hbt3);

    elm_hover_content_set(data->hv, "top", data->bx);


    data->bt3 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt3, "Show");
    evas_object_smart_callback_add(data->bt3, "clicked", message_list_view_show_clicked, data);
    window_swallow(win, "button_show", data->bt3);
    evas_object_show(data->bt3);


    g_debug("tree init");

    if(data->selected_row != NULL) {
        g_debug("Selecting row #####");
        etk_tree_row_select(data->selected_row);
    }

    if(data->scroll_range != NULL) {
        etk_range_value_set(etk_scrolled_view_vscrollbar_get(etk_tree_scrolled_view_get(ETK_TREE(data->tree_messages))), etk_range_value_get(data->scroll_range));
    }

    data->container = etk_embed_new(evas_object_evas_get(window_evas_object_get(win)));
    etk_container_add(ETK_CONTAINER(data->container), data->tree_messages);
    etk_widget_show_all(data->container);

    window_swallow(win, "swallow", etk_embed_object_get(ETK_EMBED(data->container)));
}

void messages_list_view_hide(struct MessageListViewData *data) {
    struct Window *win = data->win;

    window_unswallow(win, data->bt1);
    evas_object_del(data->bt1);

    window_unswallow(win, data->bt2);
    evas_object_del(data->bt2);

    window_unswallow(win, data->bt3);
    evas_object_del(data->bt3);

    evas_object_del(data->hbt1);
    evas_object_del(data->hbt2);
    evas_object_del(data->hbt3);
    evas_object_del(data->bx);
    evas_object_del(data->hv);



    data->scroll_range = etk_scrolled_view_vscrollbar_get(etk_tree_scrolled_view_get(ETK_TREE(data->tree_messages)));
    window_unswallow(win, data->container);
    etk_widget_hide_all(data->container);

    // TODO: Free tree memory
    //etk_object_destroy(col1);
    //etk_object_destroy(tree_messages);
    //etk_object_destroy(scrolled_view);
    //etk_object_destroy(container);
}



static void process_message(GValueArray *message, struct MessageListViewData *data) {
    GHashTable *details = g_value_get_pointer(g_value_array_get_nth(message, 4));
    long timestamp = g_value_get_long(g_hash_table_lookup(details, "timestamp_int"));
    char datestr[32];
    strftime(datestr, 31, "%e.%m.%Y, %H:%M", gmtime(&timestamp));

    GHashTable *parameters = g_hash_table_new(NULL, NULL);
    g_hash_table_insert(parameters, strdup("number"), strdup(g_value_get_string(g_value_array_get_nth(message, 2))));
    g_hash_table_insert(parameters, strdup("content"), strdup(g_value_get_string(g_value_array_get_nth(message, 3))));
    g_hash_table_insert(parameters, strdup("date"), strdup(datestr));

    Etk_Tree_Row *row = etk_tree_row_append(ETK_TREE(data->tree_messages), NULL, data->col1, parameters, NULL);
    etk_tree_row_data_set(row, g_value_array_copy(message));
}



static void add_integer_timestamp_to_message(GValueArray *message) {
    GHashTable *details = g_value_get_pointer(g_value_array_get_nth(message, 4));
    char *timestr = g_value_get_string(g_hash_table_lookup(details, "timestamp"));
    time_t timestamp = time_stringtotimestamp(timestr);

    // Insert integer timestamp into array
    GValue *value = g_slice_alloc0(sizeof(GValue));
    g_value_init(value, G_TYPE_LONG);
    g_value_set_long(value, timestamp);
    g_hash_table_insert(details, strdup("timestamp_int"), value);
}

static gint compare_messages(GValueArray **a, GValueArray **b) {
    GHashTable *h1 = g_value_get_pointer(g_value_array_get_nth(*a, 4));
    GHashTable *h2 = g_value_get_pointer(g_value_array_get_nth(*b, 4));
    
    long la = g_value_get_long(g_hash_table_lookup(h1, "timestamp_int"));
    long lb = g_value_get_long(g_hash_table_lookup(h2, "timestamp_int"));

    if(la > lb)
        return -1;
    else if(la < lb)
        return 1;
    else
        return 0;
}




static void message_list_view_answer_clicked(struct MessageListViewData *data, Evas_Object *obj, void *event_info) {
    data->selected_row = etk_tree_selected_row_get(data->tree_messages);
    if(data->selected_row != NULL) {
        GValueArray *message = etk_tree_row_data_get(data->selected_row);

        // TODO

        g_debug("message_list_view_answer_clicked()");

        //messages_mode = MODE_NEW1;
        //recipient_number = strdup(g_value_get_string(g_value_array_get_nth(message, 2)));
        //frame_show(messages_new1_show, messages_new1_hide);
    }
}


static void message_list_view_back_clicked(struct MessageListViewData *data, Evas_Object *obj, void *event_info) {
    g_debug("message_list_view_back_clicked()");

    // TODO

    //data->messages_mode = MODE_FOLDERS;
    //pipe_write(pipe_handler, messages_event, EVENT_LIST_CACHED, win);
}


static void message_list_view_show_clicked(struct MessageListViewData *data, Evas_Object *obj, void *event_info) {
    g_debug("message_list_view_show_clicked()");

    data->selected_row = etk_tree_selected_row_get(data->tree_messages);
    if(data->selected_row != NULL) {
        // TODO

        /*data->messages_mode = MODE_MESSAGE;
        data->tmp_message = etk_tree_row_data_get(data->selected_row);
        pipe_write(pipe_handler, messages_event, EVENT_MODE_MESSAGE, win);*/
    }
}



static void message_list_view_delete_clicked(struct MessageListViewData *data, Evas_Object *obj, void *event_info) {
    g_debug("message_list_view_delete_clicked()");

    data->selected_row = etk_tree_selected_row_get(data->tree_messages);
    if(data->selected_row != NULL) {
        // TODO

        /*data->messages_mode = MODE_DELETE;
        data->tmp_message = etk_tree_row_data_get(data->selected_row);
        window_frame_show(win, messages_delete_show, messages_delete_hide);*/
    }
}


static void my_hover_bt_1(void *data, Evas_Object *obj, void *event_info) {
   Evas_Object *hv = data;
   evas_object_show(hv);
}

