#include "views.h"


struct MessageListViewData {
    struct Window *win;

    Evas_Object *content_entry, *bt1, *bt2, *bt3, *hv, *bx, *button_answer, *button_delete;

    Etk_Widget *container, *tree;
    Etk_Tree_Row *selected_row;
    Etk_Tree_Col *col1;

    GPtrArray *messages;
};


static void my_hover_bt_1(void *data, Evas_Object *obj, void *event_info);
static void message_list_view_new_clicked(struct MessageListViewData *data, Evas_Object *obj, void *event_info);
static void message_list_view_show_clicked(struct MessageListViewData *data, Evas_Object *obj, void *event_info);
static void message_list_view_delete_clicked(struct MessageListViewData *data, Evas_Object *obj, void *event_info);
static void message_list_view_answer_clicked(struct MessageListViewData *data, Evas_Object *obj, void *event_info);
static void message_list_view_message_deleted(struct MessageListViewData *data);
static void retrieve_messagebook_callback(GError*error, GPtrArray*messages, struct MessageListViewData *data);
static void retrieve_messagebook_callback2(struct MessageListViewData *data);



struct MessageListViewData *message_list_view_show(struct Window *win, GHashTable *options) {
    g_debug("message_list_view_show()");

    struct MessageListViewData *data = g_slice_alloc0(sizeof(struct MessageListViewData));
    data->win = win;


    window_layout_set(win, MESSAGE_FILE, "list");
    window_text_set(win, "title", "Inbox"); 

    data->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt1, "New");
    evas_object_smart_callback_add(data->bt1, "clicked", message_list_view_new_clicked, data);
    window_swallow(win, "button_new", data->bt1);
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
    elm_box_horizontal_set(data->bx, 0);
    elm_box_homogenous_set(data->bx, 1);
    evas_object_show(data->bx);

    data->button_answer = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->button_answer, "Answer");
    evas_object_size_hint_min_set(data->button_answer, 130, 80);
    evas_object_smart_callback_add(data->button_answer, "clicked", message_list_view_answer_clicked, data);
    evas_object_show(data->button_answer);
    elm_box_pack_end(data->bx, data->button_answer);

    data->button_delete = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->button_delete, "Delete");
    evas_object_size_hint_min_set(data->button_delete, 130, 80);
    evas_object_smart_callback_add(data->button_delete, "clicked", message_list_view_delete_clicked, data);
    evas_object_show(data->button_delete);
    elm_box_pack_end(data->bx, data->button_delete);

    elm_hover_content_set(data->hv, "top", data->bx);


    data->bt3 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt3, "Show");
    evas_object_smart_callback_add(data->bt3, "clicked", message_list_view_show_clicked, data);
    window_swallow(win, "button_show", data->bt3);
    evas_object_show(data->bt3);


    g_debug("tree init");

    data->tree = etk_tree_new();
    etk_tree_rows_height_set(ETK_TREE(data->tree), 80);
    etk_tree_mode_set(ETK_TREE(data->tree), ETK_TREE_MODE_LIST);
    etk_tree_headers_visible_set(ETK_TREE(data->tree), ETK_FALSE);
    etk_tree_multiple_select_set(ETK_TREE(data->tree), ETK_FALSE);

    data->col1 = etk_tree_col_new(ETK_TREE(data->tree), "Title", 300, 0.0);
    etk_tree_col_model_add(data->col1, etk_tree_model_edje_new(MESSAGE_FILE, "message_row"));
    etk_tree_build(ETK_TREE(data->tree));

    Etk_Scrolled_View *scrolled_view = etk_tree_scrolled_view_get(ETK_TREE(data->tree));
    etk_scrolled_view_dragable_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_TRUE);
    etk_scrolled_view_drag_bouncy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_FALSE);
    etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_POLICY_HIDE, ETK_POLICY_HIDE);

    data->container = etk_embed_new(evas_object_evas_get(window_evas_object_get(win)));
    etk_container_add(ETK_CONTAINER(data->container), data->tree);
    etk_widget_show_all(data->container);

    window_swallow(win, "list", etk_embed_object_get(ETK_EMBED(data->container)));


    ogsmd_sim_retrieve_messagebook("read", retrieve_messagebook_callback, data);
    window_show(win);
    return data;
}

void message_list_view_hide(struct MessageListViewData *data) {
    struct Window *win = data->win;

    evas_object_del(data->bt1);
    evas_object_del(data->bt2);
    evas_object_del(data->bt3);

    evas_object_del(data->button_answer);
    evas_object_del(data->button_delete);
    evas_object_del(data->bx);
    evas_object_del(data->hv);

    window_unswallow(win, data->container);
    etk_widget_hide_all(data->container);

    // TODO: Free tree memory
    //etk_object_destroy(col1);
    //etk_object_destroy(tree);
    //etk_object_destroy(scrolled_view);
    //etk_object_destroy(container);
}


static void add_integer_timestamp_to_message(GValueArray *message) {
    GHashTable *details = g_value_get_boxed(g_value_array_get_nth(message, 4));
    char *timestr = g_value_get_string(g_hash_table_lookup(details, "timestamp"));
    time_t timestamp = time_stringtotimestamp(timestr);

    // Insert integer timestamp into array
    GValue *value = g_slice_alloc0(sizeof(GValue));
    g_value_init(value, G_TYPE_LONG);
    g_value_set_long(value, timestamp);
    g_hash_table_insert(details, strdup("timestamp_int"), value);
}


static gint compare_messages(GValueArray **a, GValueArray **b) {
    GHashTable *h1 = g_value_get_boxed(g_value_array_get_nth(*a, 4));
    GHashTable *h2 = g_value_get_boxed(g_value_array_get_nth(*b, 4));
    
    long la = g_value_get_long(g_hash_table_lookup(h1, "timestamp_int"));
    long lb = g_value_get_long(g_hash_table_lookup(h2, "timestamp_int"));

    if(la > lb)
        return -1;
    else if(la < lb)
        return 1;
    else
        return 0;
}


static void process_message(GValueArray *message, struct MessageListViewData *data) {
    GHashTable *details = g_value_get_boxed(g_value_array_get_nth(message, 4));
    long timestamp = g_value_get_long(g_hash_table_lookup(details, "timestamp_int"));
    char datestr[32];
    strftime(datestr, 31, "%e.%m.%Y, %H:%M", gmtime(&timestamp));

    GHashTable *parameters = g_hash_table_new(NULL, NULL);
    g_hash_table_insert(parameters, strdup("number"), strdup(g_value_get_string(g_value_array_get_nth(message, 2))));
    char *content = strdup(g_value_get_string(g_value_array_get_nth(message, 3)));
    string_replace_newline(content);
    g_hash_table_insert(parameters, strdup("content"), content);
    g_hash_table_insert(parameters, strdup("date"), strdup(datestr));

    Etk_Tree_Row *row = etk_tree_row_append(ETK_TREE(data->tree), NULL, data->col1, parameters, NULL);
    etk_tree_row_data_set(row, g_value_array_copy(message));
}


static void retrieve_messagebook_callback(GError*error, GPtrArray*messages, struct MessageListViewData *data) {
    g_debug("retrieve messagebook callback(error=%d)", error);
    data->messages = messages;
    g_ptr_array_foreach(data->messages, add_integer_timestamp_to_message, NULL);
    g_ptr_array_sort(data->messages, compare_messages);

    async_trigger(retrieve_messagebook_callback2, data);
}

static void retrieve_messagebook_callback2(struct MessageListViewData *data) {
    g_ptr_array_foreach(data->messages, process_message, data);
}




static void message_list_view_answer_clicked(struct MessageListViewData *data, Evas_Object *obj, void *event_info) {
    evas_object_hide(data->hv);

    data->selected_row = etk_tree_selected_row_get(data->tree);
    if(data->selected_row != NULL) {
        GValueArray *message = etk_tree_row_data_get(data->selected_row);

        GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
        g_hash_table_insert(options, "recipient", g_value_get_string(g_value_array_get_nth(message, 2)));

        struct Window *win = window_new("SMS Answer");
        window_init(win);
        window_view_show(win, options, message_new_view_show, message_new_view_hide);
    }
}


static void message_list_view_new_clicked(struct MessageListViewData *data, Evas_Object *obj, void *event_info) {
    g_debug("message_list_view_new_clicked()");

    struct Window *win = window_new("Compose SMS");
    window_init(win);
    window_view_show(win, NULL, message_new_view_show, message_new_view_hide);
}


static void message_list_view_show_clicked(struct MessageListViewData *data, Evas_Object *obj, void *event_info) {
    g_debug("message_list_view_show_clicked()");

    data->selected_row = etk_tree_selected_row_get(data->tree);
    if(data->selected_row != NULL) {
        GValueArray *message = etk_tree_row_data_get(data->selected_row);

        GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
        g_hash_table_insert(options, "id", g_value_get_int(g_value_array_get_nth(message, 0)));
        g_hash_table_insert(options, "delete_callback", message_list_view_message_deleted);
        g_hash_table_insert(options, "delete_callback_data", data);

        struct Window *win = window_new("Show Message");
        window_init(win);
        window_view_show(win, options, message_show_view_show, message_show_view_hide);
    }
}


static void message_list_view_delete_clicked(struct MessageListViewData *data, Evas_Object *obj, void *event_info) {
    g_debug("message_list_view_delete_clicked()");
    evas_object_hide(data->hv);

    data->selected_row = etk_tree_selected_row_get(data->tree);
    if(data->selected_row != NULL) {
        GValueArray *message = etk_tree_row_data_get(data->selected_row);

        GHashTable *options = g_hash_table_new(g_str_hash, g_str_equal);
        g_hash_table_insert(options, "id", g_value_get_int(g_value_array_get_nth(message, 0)));
        g_hash_table_insert(options, "delete_callback", message_list_view_message_deleted);
        g_hash_table_insert(options, "delete_callback_data", data);

        struct Window *win = window_new("Delete Message");
        window_init(win);
        window_view_show(win, options, message_delete_view_show, message_delete_view_hide);
    }
}


static void my_hover_bt_1(void *data, Evas_Object *obj, void *event_info) {
   Evas_Object *hv = data;
   evas_object_show(hv);
}

static void message_list_view_message_deleted(struct MessageListViewData *data) {
    // TODO: Reload list instead of deleting the selected message
    data->selected_row = etk_tree_selected_row_get(data->tree);
    if(data->selected_row != NULL) {
        etk_tree_row_delete(data->selected_row);
    }
}

