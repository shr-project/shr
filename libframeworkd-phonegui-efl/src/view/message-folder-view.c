#include "views.h"


/*
 * NOT USED AT THE MOMENT
 */


struct MessageFolderViewData {
    struct Window *win;
    int mode;

    Etk_Range *scroll_range;
    Etk_Widget *container, *tree_folders, *tree_messages;
    Etk_Tree_Row *selected_row;
    Etk_Tree_Col *col1;

    GPtrArray *tmp_messages;
    Evas_Object *bt1, *bt2, *bt3;
};



static void message_folder_view_retrieve_messagebook_callback(GError*error, GPtrArray*messages, struct MessageFolderViewData *data);
static void message_folder_view_retrieve_messagebook_callback2(struct MessageFolderViewData *data);
static void messages_button_new_clicked(struct MessageFolderViewData *data, Evas_Object *obj, void *event_info);
static void messages_button_show_clicked(struct MessageFolderViewData *data, Evas_Object *obj, void *event_info);


struct MessageFolderViewData *message_folder_view_show(struct Window *win, GHashTable *options) {
    struct MessageFolderViewData *data = g_slice_alloc0(sizeof(struct MessageFolderViewData));
    data->win = win;

    window_layout_set(win, MESSAGE_FILE, "loading");
    window_show(win);

    ogsmd_sim_retrieve_messagebook("read", message_folder_view_retrieve_messagebook_callback, data);
}

void message_folder_view_hide(struct MessageFolderViewData *data) {
    window_unswallow(data->win, data->bt1);
    evas_object_del(data->bt1);

    window_unswallow(data->win, data->bt2);
    evas_object_del(data->bt2);

    window_unswallow(data->win, data->container);
    etk_widget_hide_all(data->container);
}



static void message_folder_view_retrieve_messagebook_callback(GError*error, GPtrArray*messages, struct MessageFolderViewData *data) {
    g_debug("retrieve_messagebook_callback()");
    data->tmp_messages = messages;

    async_trigger(message_folder_view_retrieve_messagebook_callback2, data);
}


static void message_folder_view_retrieve_messagebook_callback2(struct MessageFolderViewData *data) {

    struct Window *win = data->win;

    window_layout_set(win, MESSAGE_FILE, "folders");

    data->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt1, "New");
    evas_object_smart_callback_add(data->bt1, "clicked", messages_button_new_clicked, win);
    window_swallow(win, "button_new", data->bt1);
    evas_object_show(data->bt1);

    data->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(data->bt2, "Show");
    evas_object_smart_callback_add(data->bt2, "clicked", messages_button_show_clicked, win);
    window_swallow(win, "button_show", data->bt2);
    evas_object_show(data->bt2);

    data->tree_folders = etk_tree_new();
    etk_tree_rows_height_set(ETK_TREE(data->tree_folders), 80);
    etk_tree_mode_set(ETK_TREE(data->tree_folders), ETK_TREE_MODE_LIST);
    etk_tree_headers_visible_set(ETK_TREE(data->tree_folders), ETK_FALSE);
    etk_tree_multiple_select_set(ETK_TREE(data->tree_folders), ETK_FALSE);

    data->col1 = etk_tree_col_new(ETK_TREE(data->tree_folders), "Title", 300, 0.0);
    etk_tree_col_model_add(data->col1, etk_tree_model_edje_new(MESSAGE_FILE, "folder_row"));
    etk_tree_build(ETK_TREE(data->tree_folders));

    Etk_Scrolled_View *scrolled_view = etk_tree_scrolled_view_get(ETK_TREE(data->tree_folders));
    etk_scrolled_view_dragable_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_TRUE);
    etk_scrolled_view_drag_bouncy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_FALSE);
    etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_POLICY_HIDE, ETK_POLICY_HIDE);

    // Example entry inbox
    g_debug("Messagecount: %d", data->tmp_messages->len);
    char label[32];
    sprintf(label, "%d messages", data->tmp_messages->len);
    GHashTable *parameters = g_hash_table_new(NULL, NULL);
    g_hash_table_insert(parameters, strdup("name"), strdup("Inbox"));
    g_hash_table_insert(parameters, strdup("info"), strdup(label));
    Etk_Tree_Row *row = etk_tree_row_append(ETK_TREE(data->tree_folders), NULL, data->col1, parameters, NULL);
    etk_tree_row_data_set(row, strdup("Inbox"));

    data->container = etk_embed_new(evas_object_evas_get(window_evas_object_get(win)));
    etk_container_add(ETK_CONTAINER(data->container), data->tree_folders);
    etk_widget_show_all(data->container);
    window_swallow(win, "list_swallow", etk_embed_object_get(ETK_EMBED(data->container)));

}



static void messages_button_new_clicked(struct MessageFolderViewData *data, Evas_Object *obj, void *event_info) {
    g_debug("messages_button_new_clicked()");

    struct Window *win = window_new("Compose SMS");
    window_init(win);
    window_view_show(win, NULL, message_new_view_show, message_new_view_hide);
}


static void messages_button_show_clicked(struct MessageFolderViewData *data, Evas_Object *obj, void *event_info) {
    g_debug("messages_button_show_clicked()");

    Etk_Tree_Row *row = etk_tree_selected_row_get(data->tree_folders);
    if(row != NULL) {
        // TODO

        /*data->mode = MODE_LIST;
        g_debug("before loading");
        window_frame_show(win, messages_loading_show, NULL);
        g_debug("after loading");

        data->messages_category = etk_tree_row_data_get(row);
        g_debug("got row data");
        pipe_write(pipe_handler, messages_event, EVENT_LIST_CACHED, win);
        g_debug("EVENT_LIST_CACHED sent");*/
    }

}

