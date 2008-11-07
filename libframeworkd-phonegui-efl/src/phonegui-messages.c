#include "phonegui-messages.h"
#include <pthread.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <Edje.h>
#include <etk/Etk.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>
#include "kbd.h"
#include "time-helper.h"
#include "phonegui-init.h"
#include "etk_tree_model_edje.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/messages.edj"

typedef enum {
    MODE_FOLDERS,
    MODE_LIST,
    MODE_MESSAGE,
    MODE_NEW1,
    MODE_NEW2,
    MODE_DELETE
} MessagesModes;

typedef enum {
    EVENT_SHOW,
    EVENT_MODE_FOLDERS,
    EVENT_MODE_LIST,
    EVENT_MODE_MESSAGE,
    EVENT_HIDE,
    EVENT_LIST_CACHED
} MessagesEvents;


static MessagesModes messages_mode;

static Etk_Widget *container = NULL;
static Etk_Widget *tree;
static Etk_Tree_Col *col1;

static Etk_Entry *number_entry;

static Etk_Widget *container_number;


static char *messages_category;

static GPtrArray *tmp_messages;
static int tmp_id;
static char *tmp_number;
static char *tmp_content;
static char *tmp_status;
static char *tmp_date;

static char *recipient_number = NULL;
static char *new_message_content;

static Evas_Object *content_entry, *bt1, *bt2, *bt3;


void phonegui_messages_show(int argc, char** argv) {
    g_debug("phonegui_messages_show()");

    messages_mode = MODE_FOLDERS;
    pipe_write(pipe_handler, messages_event, EVENT_SHOW);
    pipe_write(pipe_handler, messages_event, EVENT_MODE_FOLDERS);
}

void phonegui_messages_hide() {
    pipe_write(pipe_handler, messages_event, EVENT_HIDE);
}


void messages_delete(void *data, Evas_Object *obj, void *event_info) {
    phonegui_messages_hide();
}


void messages_input(void *data, Evas_Object *obj, const char *emission, const char *source) {
    g_debug("messages_input() input: %s", emission);
}



void add_integer_timestamp_to_message(GValueArray *message) {
    GHashTable *details = g_value_get_pointer(g_value_array_get_nth(message, 4));
    char *timestr = g_value_get_string(g_hash_table_lookup(details, "timestamp"));
    //g_debug("datestr: %s", datestr);
    time_t timestamp = time_stringtotimestamp(timestr);

    // Insert integer timestamp into array
    GValue *value = g_slice_alloc0(sizeof(GValue));
    g_value_init(value, G_TYPE_LONG);
    g_value_set_long(value, timestamp);
    //g_debug("%ld timestamp", g_value_get_long(timestamp));
    g_hash_table_insert(details, strdup("timestamp_int"), value);
}

gint compare_messages(GValueArray **a, GValueArray **b) {
    GHashTable *h1 = g_value_get_pointer(g_value_array_get_nth(*a, 4));
    GHashTable *h2 = g_value_get_pointer(g_value_array_get_nth(*b, 4));
    
    long la = g_value_get_long(g_hash_table_lookup(h1, "timestamp_int"));
    long lb = g_value_get_long(g_hash_table_lookup(h2, "timestamp_int"));

    //g_debug("## VALUES: %s, %s", g_value_get_string(g_hash_table_lookup(h1, "timestamp")), g_value_get_string(g_hash_table_lookup(h2, "timestamp")));
    //g_debug("   VALUES: %ld, %ld", la, lb);    

    if(la > lb)
        return -1;
    else if(la < lb)
        return 1;
    else
        return 0;
}



void process_message(GValueArray *message) {
    GHashTable *details = g_value_get_pointer(g_value_array_get_nth(message, 4));
    long timestamp = g_value_get_long(g_hash_table_lookup(details, "timestamp_int"));
    char datestr[32];
    strftime(datestr, 31, "%e.%m.%Y, %H:%M", gmtime(&timestamp));

    int *id = g_slice_alloc(sizeof(int));
    *id = g_value_get_int(g_value_array_get_nth(message, 0));

    GHashTable *parameters = g_hash_table_new(NULL, NULL);
    g_hash_table_insert(parameters, strdup("number"), strdup(g_value_get_string(g_value_array_get_nth(message, 2))));
    g_hash_table_insert(parameters, strdup("content"), strdup(g_value_get_string(g_value_array_get_nth(message, 3))));
    g_hash_table_insert(parameters, strdup("date"), strdup(datestr));
    Etk_Tree_Row *row = etk_tree_row_append(ETK_TREE(tree), NULL, col1, parameters, NULL);
    etk_tree_row_data_set(row, id);
}


void retrieve_messagebook_callback(GError*error, GPtrArray*messages, gpointer userdata) {
    g_debug("retrieve messagebook callback");
    tmp_messages = messages;
    g_ptr_array_foreach(tmp_messages, add_integer_timestamp_to_message, NULL);
    g_ptr_array_sort(tmp_messages, compare_messages);
    pipe_write(pipe_handler, messages_event, EVENT_LIST_CACHED);
}


void retrieve_message_callback(GError *error, char *status, char *number, char *content, GHashTable *properties, gpointer data) {
    tmp_status = strdup(status);
    tmp_number = strdup(number);
    tmp_content = strdup(content);

    GValue *value = g_hash_table_lookup(properties, "timestamp");
    if(value != NULL) {
        time_t timestamp = time_stringtotimestamp(g_value_get_string(value));
        char timestr[32];
        strftime(timestr, 31, "%a, %e.%m.%Y, %H:%M", gmtime(&timestamp));
        tmp_date = strdup(timestr);
    }

    pipe_write(pipe_handler, messages_event, EVENT_MODE_MESSAGE);
}

void delete_message_callback(GError *error, gpointer userdata) {
    messages_mode = MODE_LIST;
    pipe_write(pipe_handler, messages_event, EVENT_MODE_LIST);
}

void send_message_callback(GError *error, int transaction_index, gpointer userdata) {
    messages_mode = MODE_LIST;
    pipe_write(pipe_handler, messages_event, EVENT_LIST_CACHED);
}


void messages_event(int event) {
    g_debug("messages_event()");
    g_debug("Event: %d", event);

    if(event == EVENT_SHOW) {
        window_create("Messages", messages_input, messages_event, messages_delete);
    } else if(event == EVENT_MODE_FOLDERS) {
        elm_layout_file_set(layout, UI_FILE, "loading");
        evas_object_show(win);
        ogsmd_sim_retrieve_messagebook("read", retrieve_messagebook_callback, NULL);
    } else if(event == EVENT_MODE_MESSAGE) {
        frame_show(messages_message_show, messages_message_hide);
    } else if(event == EVENT_MODE_LIST) {
        elm_layout_file_set(layout, UI_FILE, "loading");
        ogsmd_sim_retrieve_messagebook("read", retrieve_messagebook_callback, NULL);
    } else if(event == EVENT_LIST_CACHED) {
        g_debug("event = EVENT_LIST_CACHED");
        if(messages_mode == MODE_FOLDERS) {
            g_debug("MODE_FOLDERS");
            frame_show(messages_folders_show, messages_folders_hide);
        } else {
            g_debug("MODE_LIST");
            frame_show(messages_list_show, messages_list_hide);
        }
    } else if(event == EVENT_HIDE) {
        evas_object_hide(win);
        phonegui_exit_cb();
    } else {
        g_error("Unknown event %d", event);
    }
}



/*
 * Button triggers
 */
void messages_button_options_clicked() {
    // TODO
}

void messages_button_answer_clicked() {
    recipient_number = strdup(tmp_number);
    messages_mode = MODE_NEW1;
    frame_show(messages_new1_show, messages_new1_hide);
}

void messages_button_new_clicked() {
    messages_mode = MODE_NEW1;
    frame_show(messages_new1_show, messages_new1_hide);
}

void messages_button_show_clicked() {
    g_debug("messages_button_show_clicked()");

    if(messages_mode == MODE_FOLDERS) {
        g_debug("MODE_FOLDERS");
        Etk_Tree_Row *row = etk_tree_selected_row_get(tree);
        if(row != NULL) {
            messages_mode = MODE_LIST;
            g_debug("before loading");
            frame_show(messages_loading_show, NULL);
            g_debug("after loading");

            messages_category = etk_tree_row_data_get(row);
            g_debug("got row data");
            pipe_write(pipe_handler, messages_event, EVENT_LIST_CACHED);
            g_debug("EVENT_LIST_CACHED sent");
        }
    } else if(messages_mode == MODE_LIST) {
        g_debug("MODE_LIST");
        Etk_Tree_Row *row = etk_tree_selected_row_get(tree);
        if(row != NULL) {
            messages_mode = MODE_MESSAGE;
            frame_show(messages_loading_show, NULL);

            int *id = etk_tree_row_data_get(row);
            ogsmd_sim_retrieve_message(*id, retrieve_message_callback, NULL);
        }
    }
}

void messages_button_back_clicked() {
    if(messages_mode == MODE_MESSAGE || messages_mode == MODE_DELETE) {
        messages_mode = MODE_LIST;
        pipe_write(pipe_handler, messages_event, EVENT_LIST_CACHED);
    } else if(messages_mode == MODE_LIST || messages_mode == MODE_NEW1) {
        messages_mode = MODE_FOLDERS;
        pipe_write(pipe_handler, messages_event, EVENT_LIST_CACHED);
    } else if(messages_mode == MODE_NEW2) {
        messages_mode = MODE_NEW1;
        frame_show(messages_new1_show, messages_new1_hide);
    }
}

void messages_button_continue_clicked() {
    if(messages_mode == MODE_NEW1) {
        messages_mode = MODE_NEW2;
        frame_show(messages_new2_show, messages_new2_hide);
    } else if(messages_mode == MODE_NEW2) {
        frame_show(messages_loading_show, NULL);

        char *number = etk_entry_text_get(number_entry);
        g_debug("Sending to %s:\n%s", number, new_message_content);

        // TOD: Add setting to enable SMS reports
        GHashTable *properties;
        properties = g_hash_table_new(NULL, NULL);
        ogsmd_sms_send_message(strdup(number), new_message_content, properties, send_message_callback, NULL);
    }
}

void messages_button_delete_clicked() {
    Etk_Tree_Row *row = etk_tree_selected_row_get(tree);
    if(row != NULL) {
        int *id = etk_tree_row_data_get(row);
        tmp_id = *id;

        messages_mode = MODE_DELETE;
        frame_show(messages_delete_show, messages_delete_hide);
    }
}

void messages_button_yes_clicked() {
    frame_show(messages_loading_show, NULL);
    ogsmd_sim_delete_message(tmp_id, delete_message_callback, NULL);
}

void messages_button_no_clicked() {
    messages_mode = MODE_LIST;
    pipe_write(pipe_handler, messages_event, EVENT_LIST_CACHED);
}




/*
 * Views
 */
void messages_folders_show() {
    elm_layout_file_set(layout, UI_FILE, "folders");

    bt1 = elm_button_add(win);
    elm_button_label_set(bt1, "New");
    evas_object_smart_callback_add(bt1, "clicked", messages_button_new_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_new", bt1);
    evas_object_show(bt1);

    bt2 = elm_button_add(win);
    elm_button_label_set(bt2, "Show");
    evas_object_smart_callback_add(bt2, "clicked", messages_button_show_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_show", bt2);
    evas_object_show(bt2);

    tree = etk_tree_new();
    etk_tree_rows_height_set(ETK_TREE(tree), 80);
    etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_LIST);
    etk_tree_headers_visible_set(ETK_TREE(tree), ETK_FALSE);
    etk_tree_multiple_select_set(ETK_TREE(tree), ETK_FALSE);

    col1 = etk_tree_col_new(ETK_TREE(tree), "Title", 300, 0.0);
    etk_tree_col_model_add(col1, etk_tree_model_edje_new(UI_FILE, "folder_row"));
    etk_tree_build(ETK_TREE(tree));

    Etk_Scrolled_View *scrolled_view = etk_tree_scrolled_view_get(ETK_TREE(tree));
    etk_scrolled_view_dragable_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_TRUE);
    etk_scrolled_view_drag_bouncy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_FALSE);
    etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_POLICY_HIDE, ETK_POLICY_HIDE);

    // Example entry inbox
    g_debug("Messagecount: %d", tmp_messages->len);
    char label[32];
    sprintf(label, "%d messages", tmp_messages->len);
    GHashTable *parameters = g_hash_table_new(NULL, NULL);
    g_hash_table_insert(parameters, strdup("name"), strdup("Inbox"));
    g_hash_table_insert(parameters, strdup("info"), strdup(label));
    Etk_Tree_Row *row = etk_tree_row_append(ETK_TREE(tree), NULL, col1, parameters, NULL);
    etk_tree_row_data_set(row, strdup("Inbox"));

    container = etk_embed_new(evas_object_evas_get(win));
    etk_container_add(ETK_CONTAINER(container), tree);
    etk_widget_show_all(container);
    edje_object_part_swallow(elm_layout_edje_get(layout), "list_swallow", etk_embed_object_get(ETK_EMBED(container)));
}

void messages_folders_hide() {
    edje_object_part_unswallow(elm_layout_edje_get(layout), bt1);
    evas_object_del(bt1);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt2);
    evas_object_del(bt2);

    edje_object_part_unswallow(elm_layout_edje_get(layout), container);
    etk_widget_hide_all(container);
}


void messages_list_show() {
    g_debug("messages_list_show()");

    elm_layout_file_set(layout, UI_FILE, "list");
    edje_object_part_text_set(elm_layout_edje_get(layout), "title", "Inbox"); 

    bt1 = elm_button_add(win);
    elm_button_label_set(bt1, "Back");
    evas_object_smart_callback_add(bt1, "clicked", messages_button_back_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_back", bt1);
    evas_object_show(bt1);

    bt2 = elm_button_add(win);
    elm_button_label_set(bt2, "Delete");
    evas_object_smart_callback_add(bt2, "clicked", messages_button_delete_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_delete", bt2);
    evas_object_show(bt2);

    bt3 = elm_button_add(win);
    elm_button_label_set(bt3, "Show");
    evas_object_smart_callback_add(bt3, "clicked", messages_button_show_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_show", bt3);
    evas_object_show(bt3);

g_debug("tree init");
    tree = etk_tree_new();
    etk_tree_rows_height_set(ETK_TREE(tree), 80);
    etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_LIST);
    etk_tree_headers_visible_set(ETK_TREE(tree), ETK_FALSE);
    etk_tree_multiple_select_set(ETK_TREE(tree), ETK_FALSE);

    col1 = etk_tree_col_new(ETK_TREE(tree), "Title", 300, 0.0);
    etk_tree_col_model_add(col1, etk_tree_model_edje_new(UI_FILE, "message_row"));
    etk_tree_build(ETK_TREE(tree));

    Etk_Scrolled_View *scrolled_view = etk_tree_scrolled_view_get(ETK_TREE(tree));
    etk_scrolled_view_dragable_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_TRUE);
    etk_scrolled_view_drag_bouncy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_FALSE);
    etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_POLICY_HIDE, ETK_POLICY_HIDE);

g_debug("before foreach's");

    g_ptr_array_foreach(tmp_messages, process_message, NULL);

g_debug("after");

    container = etk_embed_new(evas_object_evas_get(win));
    etk_container_add(ETK_CONTAINER(container), tree);
    etk_widget_show_all(container);

    edje_object_part_swallow(elm_layout_edje_get(layout), "swallow", etk_embed_object_get(ETK_EMBED(container)));
}

void messages_list_hide() {
    edje_object_part_unswallow(elm_layout_edje_get(layout), bt1);
    evas_object_del(bt1);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt2);
    evas_object_del(bt2);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt3);
    evas_object_del(bt3);

    edje_object_part_unswallow(elm_layout_edje_get(layout), container);
    etk_widget_hide_all(container);

    // TODO: Free tree memory
    //etk_object_destroy(col1);
    //etk_object_destroy(tree);
    //etk_object_destroy(scrolled_view);
    //etk_object_destroy(container);
}

void messages_message_show() {
    elm_layout_file_set(layout, UI_FILE, "message_show");
    edje_object_part_text_set(elm_layout_edje_get(layout), "message_show_status", tmp_status);
    edje_object_part_text_set(elm_layout_edje_get(layout), "message_show_number", tmp_number);
    edje_object_part_text_set(elm_layout_edje_get(layout), "message_show_content", tmp_content);
    edje_object_part_text_set(elm_layout_edje_get(layout), "message_show_date", tmp_date);

    bt1 = elm_button_add(win);
    elm_button_label_set(bt1, "Back");
    evas_object_smart_callback_add(bt1, "clicked", messages_button_back_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_back", bt1);
    evas_object_show(bt1);

    bt2 = elm_button_add(win);
    elm_button_label_set(bt2, "Options");
    evas_object_smart_callback_add(bt2, "clicked", messages_button_options_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_options", bt2);
    evas_object_show(bt2);

    bt3 = elm_button_add(win);
    elm_button_label_set(bt3, "Answer");
    evas_object_smart_callback_add(bt3, "clicked", messages_button_answer_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_answer", bt3);
    evas_object_show(bt3);
}

void messages_message_hide() {
    edje_object_part_unswallow(elm_layout_edje_get(layout), bt1);
    evas_object_del(bt1);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt2);
    evas_object_del(bt2);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt3);
    evas_object_del(bt3);
}


void messages_loading_show() {
    elm_layout_file_set(layout, UI_FILE, "loading");
}

void messages_new1_show() {
    elm_layout_file_set(layout, UI_FILE, "content_edit");

    bt1 = elm_button_add(win);
    elm_button_label_set(bt1, "Back");
    evas_object_smart_callback_add(bt1, "clicked", messages_button_back_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_back", bt1);
    evas_object_show(bt1);

    bt2 = elm_button_add(win);
    elm_button_label_set(bt2, "Continue");
    evas_object_smart_callback_add(bt2, "clicked", messages_button_continue_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_continue", bt2);
    evas_object_show(bt2);

    content_entry = elm_entry_add(win);
    elm_entry_entry_set(content_entry, "Enter your SMS here");
    edje_object_part_swallow(elm_layout_edje_get(layout), "content_entry", content_entry);
    evas_object_show(content_entry);
    elm_widget_focus_set(content_entry, 1);

    kbd_show(KEYBOARD_ALPHA);
}

void messages_new1_hide() {
    edje_object_part_unswallow(elm_layout_edje_get(layout), bt1);
    evas_object_del(bt1);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt2);
    evas_object_del(bt2);

    new_message_content = elm_entry_entry_get(content_entry);
    edje_object_part_unswallow(elm_layout_edje_get(layout), content_entry);
    evas_object_del(content_entry);

    kbd_hide();
}


void messages_new2_show() {
    elm_layout_file_set(layout, UI_FILE, "number_edit");

    bt1 = elm_button_add(win);
    elm_button_label_set(bt1, "Back");
    evas_object_smart_callback_add(bt1, "clicked", messages_button_back_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_back", bt1);
    evas_object_show(bt1);

    bt2 = elm_button_add(win);
    elm_button_label_set(bt2, "Continue");
    evas_object_smart_callback_add(bt2, "clicked", messages_button_continue_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_continue", bt2);
    evas_object_show(bt2);

    number_entry = etk_entry_new();
    if(recipient_number != NULL) {
        etk_entry_text_set(number_entry, recipient_number);
        free(recipient_number);
        recipient_number = NULL;
    }
    container_number = etk_embed_new(evas_object_evas_get(win));
    etk_container_add(ETK_CONTAINER(container_number), number_entry);
    etk_widget_show_all(container_number);
    edje_object_part_swallow(elm_layout_edje_get(layout), "number", etk_embed_object_get(ETK_EMBED(container_number)));

    kbd_show(KEYBOARD_NUMERIC);
}

void messages_new2_hide() {
    edje_object_part_unswallow(elm_layout_edje_get(layout), bt1);
    evas_object_del(bt1);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt2);
    evas_object_del(bt2);

    edje_object_part_unswallow(elm_layout_edje_get(layout), container_number);
    etk_widget_hide_all(container_number);

    kbd_hide();
}

void messages_delete_show() {
    elm_layout_file_set(layout, UI_FILE, "delete");

    bt1 = elm_button_add(win);
    elm_button_label_set(bt1, "Yes");
    evas_object_smart_callback_add(bt1, "clicked", messages_button_yes_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_yes", bt1);
    evas_object_show(bt1);

    bt2 = elm_button_add(win);
    elm_button_label_set(bt2, "No");
    evas_object_smart_callback_add(bt2, "clicked", messages_button_no_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_no", bt2);
    evas_object_show(bt2);
}

void messages_delete_hide() {
    edje_object_part_unswallow(elm_layout_edje_get(layout), bt1);
    evas_object_del(bt1);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt2);
    evas_object_del(bt2);
}


