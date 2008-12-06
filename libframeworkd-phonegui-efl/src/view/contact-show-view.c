#include "views.h"


void contacts_button_call_clicked(struct ContactsWindow *win, Evas_Object *obj, void *event_info) {
    if(win->contacts_mode == MODE_LIST) {
        Etk_Tree_Row *row = etk_tree_selected_row_get(win->tree);
        if(row != NULL) {
            GValueArray *data = etk_tree_row_data_get(row);
            char *number = g_value_get_string(g_value_array_get_nth(data, 2));
            ogsmd_call_initiate(number, "voice", NULL, NULL);
        }
    } else if(win->contacts_mode == MODE_OPTIONS) {
        char *number = g_value_get_string(g_value_array_get_nth(win->tmp_entry, 2));
        ogsmd_call_initiate(number, "voice", NULL, NULL);
    }
}

void contacts_button_options_clicked(struct ContactsWindow *win, Evas_Object *obj, void *event_info) {
    Etk_Tree_Row *row = etk_tree_selected_row_get(win->tree);
    if(row != NULL) {
        win->contacts_mode = MODE_OPTIONS;
        win->tmp_entry = etk_tree_row_data_get(row);
        window_frame_show(win, contacts_options_show, contacts_options_hide);
    }
}

void contacts_button_save_clicked(struct ContactsWindow *win, Evas_Object *obj, void *event_info) {
    if(strcmp(etk_entry_text_get(win->entry_name), "") && strcmp(etk_entry_text_get(win->entry_number), "")) {
        if(win->contacts_mode == MODE_NEW) {
            g_debug("NEW");
            ogsmd_sim_store_entry(
                "contacts",
                free_entry_index,
                etk_entry_text_get(win->entry_name),
                etk_entry_text_get(win->entry_number),
                NULL,
                NULL
            );
        } else if(win->contacts_mode == MODE_MODIFY) {
            g_debug("MODIFY");
            ogsmd_sim_store_entry(
                "contacts",
                g_value_get_int(g_value_array_get_nth(win->tmp_entry, 0)),
                etk_entry_text_get(win->entry_name),
                etk_entry_text_get(win->entry_number),
                NULL,
                NULL
            );
        }
        g_debug("SAVED");
        pipe_write(pipe_handler, contacts_event, EVENT_MODE_LIST, win);
    }
}

void contacts_button_back_clicked(struct ContactsWindow *win, Evas_Object *obj, void *event_info) {
    win->contacts_mode = MODE_LIST;
    window_frame_show(win, contacts_list_show, contacts_list_hide);
}

void contacts_button_delete_clicked(struct ContactsWindow *win, Evas_Object *obj, void *event_info) {
    window_frame_show(win, contacts_delete_show, contacts_delete_hide);
}

void contacts_button_delete_yes_clicked(struct ContactsWindow *win, Evas_Object *obj, void *event_info) {
    ogsmd_sim_delete_entry(
        "contacts",
        g_value_get_int(g_value_array_get_nth(win->tmp_entry, 0)),
        NULL,
        NULL
    );
    pipe_write(pipe_handler, contacts_event, EVENT_MODE_LIST, win);
}

void contacts_button_delete_no_clicked(struct ContactsWindow *win, Evas_Object *obj, void *event_info) {
    pipe_write(pipe_handler, contacts_event, EVENT_MODE_LIST_CACHED, win);
}

void contacts_button_close_clicked(struct ContactsWindow *win, Evas_Object *obj, void *event_info) {
    pipe_write(pipe_handler, contacts_event, EVENT_MODE_LIST_CACHED, win);
}


// Views

void contacts_list_show(struct ContactsWindow *win) {
    window_layout_set(win, UI_FILE, "list");

    win->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(win->bt1, "New");
    evas_object_smart_callback_add(win->bt1, "clicked", contacts_button_new_clicked, win);
    window_swallow(win, "button_new", win->bt1);
    evas_object_show(win->bt1);

    win->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(win->bt2, "Call");
    evas_object_smart_callback_add(win->bt2, "clicked", contacts_button_call_clicked, win);
    window_swallow(win, "button_call", win->bt2);
    evas_object_show(win->bt2);

    win->bt3 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(win->bt3, "Options");
    evas_object_smart_callback_add(win->bt3, "clicked", contacts_button_options_clicked, win);
    window_swallow(win, "button_options", win->bt3);
    evas_object_show(win->bt3);

    
    win->tree = etk_tree_new();
    etk_tree_rows_height_set(ETK_TREE(win->tree), 80);
    etk_tree_mode_set(ETK_TREE(win->tree), ETK_TREE_MODE_LIST);
    etk_tree_headers_visible_set(ETK_TREE(win->tree), ETK_FALSE);
    etk_tree_multiple_select_set(ETK_TREE(win->tree), ETK_FALSE);

    win->col1 = etk_tree_col_new(ETK_TREE(win->tree), "Title", 300, 0.0);
    etk_tree_col_model_add(win->col1, etk_tree_model_edje_new(UI_FILE, "row"));
    etk_tree_build(ETK_TREE(win->tree));

    Etk_Scrolled_View *scrolled_view = etk_tree_scrolled_view_get(ETK_TREE(win->tree));
    etk_scrolled_view_dragable_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_TRUE);
    etk_scrolled_view_drag_damping_set(ETK_SCROLLED_VIEW(scrolled_view), 400);
    etk_scrolled_view_drag_bouncy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_FALSE);
    //etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_POLICY_HIDE, ETK_POLICY_HIDE);

    // Sort entries
    g_ptr_array_sort(win->tmp_entries, compare_entries);

    // Process entries
    g_ptr_array_foreach(win->tmp_entries, process_entry, win);

    win->container = etk_embed_new(evas_object_evas_get(window_evas_object_get(win)));
    etk_container_add(ETK_CONTAINER(win->container), win->tree);
    etk_widget_show_all(win->container);
    window_swallow(win, "swallow", etk_embed_object_get(ETK_EMBED(win->container)));

    g_debug("Swallowed!");
}

void contacts_list_hide(struct ContactsWindow *win) {
    window_unswallow(win, win->bt1);
    evas_object_del(win->bt1);

    window_unswallow(win, win->bt2);
    evas_object_del(win->bt2);

    window_unswallow(win, win->bt3);
    evas_object_del(win->bt3);

    window_unswallow(win, win->container);
    etk_widget_hide_all(win->container);
}

void contacts_options_show(struct ContactsWindow *win) {
    window_layout_set(win, UI_FILE, "options");
    window_text_set(win, "name", g_value_get_string(g_value_array_get_nth(win->tmp_entry, 1)));
    window_text_set(win, "number", g_value_get_string(g_value_array_get_nth(win->tmp_entry, 2)));

    win->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(win->bt1, "Back");
    evas_object_smart_callback_add(win->bt1, "clicked", contacts_button_back_clicked, win);
    window_swallow(win, "button_back", win->bt1);
    evas_object_show(win->bt1);

    win->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(win->bt2, "Call");
    evas_object_smart_callback_add(win->bt2, "clicked", contacts_button_call_clicked, win);
    window_swallow(win, "button_call", win->bt2);
    evas_object_show(win->bt2);

    win->bt3 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(win->bt3, "Modify");
    evas_object_smart_callback_add(win->bt3, "clicked", contacts_button_modify_clicked, win);
    window_swallow(win, "button_modify", win->bt3);
    evas_object_show(win->bt3);

    win->bt4 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(win->bt4, "Delete");
    evas_object_smart_callback_add(win->bt4, "clicked", contacts_button_delete_clicked, win);
    window_swallow(win, "button_delete", win->bt4);
    evas_object_show(win->bt4);
}

void contacts_options_hide(struct ContactsWindow *win) {
    window_unswallow(win, win->bt1);
    evas_object_del(win->bt1);

    window_unswallow(win, win->bt2);
    evas_object_del(win->bt2);

    window_unswallow(win, win->bt3);
    evas_object_del(win->bt3);

    window_unswallow(win, win->bt4);
    evas_object_del(win->bt4);
}

void contacts_loading_show(struct ContactsWindow *win) {
    window_layout_set(win, UI_FILE, "loading");
}

void contacts_modify_show(struct ContactsWindow *win) {
    contacts_new_hide(win);

    window_text_set(win, "title", "Modify Contact");

    etk_entry_text_set(win->entry_number, g_value_get_string(g_value_array_get_nth(win->tmp_entry, 2)));
    etk_entry_text_set(win->entry_name, g_value_get_string(g_value_array_get_nth(win->tmp_entry, 1)));
}

void contacts_modify_hide(struct ContactsWindow *win) {
    contacts_new_hide(win);
}

void contacts_new_show(struct ContactsWindow *win) {
    window_kbd_show(win, KEYBOARD_ALPHA);

    window_layout_set(win, UI_FILE, "modify");
    window_text_set(win, "title", "New Contact");

    win->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(win->bt1, "Back");
    evas_object_smart_callback_add(win->bt1, "clicked", contacts_button_back_clicked, win);
    window_swallow(win, "button_back", win->bt1);
    evas_object_show(win->bt1);

    win->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(win->bt2, "Save");
    evas_object_smart_callback_add(win->bt2, "clicked", contacts_button_save_clicked, win);
    window_swallow(win, "button_save", win->bt2);
    evas_object_show(win->bt2);

    win->entry_number = etk_entry_new();
    win->container_number = etk_embed_new(evas_object_evas_get(window_evas_object_get(win)));
    etk_container_add(ETK_CONTAINER(win->container_number), win->entry_number);
    etk_widget_show_all(win->container_number);
    window_swallow(win, "number", etk_embed_object_get(ETK_EMBED(win->container_number)));

    win->entry_name = etk_entry_new();
    win->container_name = etk_embed_new(evas_object_evas_get(window_evas_object_get(win)));
    etk_container_add(ETK_CONTAINER(win->container_name), win->entry_name);
    etk_widget_show_all(win->container_name);
    window_swallow(win, "name", etk_embed_object_get(ETK_EMBED(win->container_name)));
}

void contacts_new_hide(struct ContactsWindow *win) {
    window_unswallow(win, win->bt1);
    evas_object_del(win->bt1);

    window_unswallow(win, win->bt2);
    evas_object_del(win->bt2);

    window_unswallow(win, win->container_number);
    etk_widget_hide_all(win->container_number);

    window_unswallow(win, win->container_name);
    etk_widget_hide_all(win->container_name);

    window_kbd_hide(win);
}

void contacts_delete_show(struct ContactsWindow *win) {
    window_layout_set(win, UI_FILE, "delete");

    win->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(win->bt1, "Yes");
    evas_object_smart_callback_add(win->bt1, "clicked", contacts_button_delete_yes_clicked, win);
    window_swallow(win, "button_yes", win->bt1);
    evas_object_show(win->bt1);

    win->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(win->bt2, "No");
    evas_object_smart_callback_add(win->bt2, "clicked", contacts_button_delete_no_clicked, win);
    window_swallow(win, "button_no", win->bt2);
    evas_object_show(win->bt2);
}

void contacts_delete_hide(struct ContactsWindow *win) {
    window_unswallow(win, win->bt1);
    evas_object_del(win->bt1);

    window_unswallow(win, win->bt2);
    evas_object_del(win->bt2);
}

void contacts_sim_full_show(struct ContactsWindow *win) {
    window_layout_set(win, UI_FILE, "dialog");
    window_text_set(win, "content", "Your storage is full. Before adding new contacts, you have to delete some old ones.");

    win->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(win->bt1, "Close");
    evas_object_smart_callback_add(win->bt1, "clicked", contacts_button_close_clicked, win);
    window_swallow(win, "button_close", win->bt1);
    evas_object_show(win->bt1);
}

void contacts_sim_full_hide(struct ContactsWindow *win) {
    window_unswallow(win, win->bt1);
    evas_object_del(win->bt1);
}
