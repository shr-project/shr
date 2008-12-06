

void delete_message_callback(GError *error, struct MessagesWindow *win) {
    win->selected_row = NULL;
    win->messages_mode = MODE_LIST;
    pipe_write(pipe_handler, messages_event, EVENT_MODE_LIST, win);
}


void messages_button_yes_clicked(struct MessagesWindow *win, Evas_Object *obj, void *event_info) {
    window_frame_show(win, messages_loading_show, NULL);
    ogsmd_sim_delete_message(g_value_get_int(g_value_array_get_nth(win->tmp_message, 0)), delete_message_callback, win);
}

void messages_button_no_clicked(struct MessagesWindow *win, Evas_Object *obj, void *event_info) {
    pipe_write(pipe_handler, messages_event, EVENT_MODE_MESSAGE, win);
}




void messages_delete_show(struct MessagesWindow *win) {
    window_layout_set(win, UI_FILE, "delete");

    win->bt1 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(win->bt1, "Yes");
    evas_object_smart_callback_add(win->bt1, "clicked", messages_button_yes_clicked, win);
    window_swallow(win, "button_yes", win->bt1);
    evas_object_show(win->bt1);

    win->bt2 = elm_button_add(window_evas_object_get(win));
    elm_button_label_set(win->bt2, "No");
    evas_object_smart_callback_add(win->bt2, "clicked", messages_button_no_clicked, win);
    window_swallow(win, "button_no", win->bt2);
    evas_object_show(win->bt2);
}

void messages_delete_hide(struct MessagesWindow *win) {
    window_unswallow(win, win->bt1);
    evas_object_del(win->bt1);

    window_unswallow(win, win->bt2);
    evas_object_del(win->bt2);
}

