#include "phonegui-messages.h"
#include <pthread.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <Edje.h>
#include <etk/Etk.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>
#include "kbd.h"
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

static Etk_Text_View *content_text_view;
static Etk_Entry *number_entry;

static Etk_Widget *container_content;
static Etk_Widget *container_number;


static char *messages_category;

// TODO: Remove tmp variables
static GPtrArray *tmp_messages;
static int tmp_id;
static char *tmp_number;
static char *tmp_content;
static char *tmp_status;
static char *tmp_date;

void phonegui_messages_show(int argc, char** argv) {
    g_debug("phonegui_messages_show()");

    messages_mode = MODE_FOLDERS;
    pipe_write(pipe_handler, messages_event, EVENT_SHOW);
    pipe_write(pipe_handler, messages_event, EVENT_MODE_FOLDERS);
}

void phonegui_messages_hide() {
}



void messages_input(void *data, Evas_Object *obj, const char *emission, const char *source) {
    g_debug("messages_input() input: %s", emission);

    if(!strcmp(emission, "options")) {
        // TODO
    } else if(!strcmp(emission, "show")) {
        if(messages_mode == MODE_FOLDERS) {
            Etk_Tree_Row *row = etk_tree_selected_row_get(tree);
            if(row != NULL) {
                messages_mode = MODE_LIST;
                frame_show(messages_loading_show, NULL);

                messages_category = etk_tree_row_data_get(row);
                pipe_write(pipe_handler, messages_event, EVENT_LIST_CACHED);
            }
        } else if(messages_mode == MODE_LIST) {
            Etk_Tree_Row *row = etk_tree_selected_row_get(tree);
            if(row != NULL) {
                messages_mode = MODE_MESSAGE;
                frame_show(messages_loading_show, NULL);

                int *id = etk_tree_row_data_get(row);
                ogsmd_sim_retrieve_message(*id, retrieve_message_callback, NULL);
            }
        }
    } else if(!strcmp(emission, "delete")) {
        Etk_Tree_Row *row = etk_tree_selected_row_get(tree);
        if(row != NULL) {
            int *id = etk_tree_row_data_get(row);
            tmp_id = *id;

            messages_mode = MODE_DELETE;
            frame_show(messages_delete_show, NULL);
        }
    } else if(!strcmp(emission, "yes")) {
        frame_show(messages_loading_show, NULL);
        ogsmd_sim_delete_message(tmp_id, delete_message_callback, NULL);
    } else if(!strcmp(emission, "no")) {
        messages_mode = MODE_LIST;
        pipe_write(pipe_handler, messages_event, EVENT_LIST_CACHED);
    } else if(!strcmp(emission, "back")) {
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
    } else if(!strcmp(emission, "answer")) {
        // TODO
    } else if(!strcmp(emission, "new")) {
        messages_mode = MODE_NEW1;
        frame_show(messages_new1_show, messages_new1_hide);
    } else if(!strcmp(emission, "continue")) {
        if(messages_mode == MODE_NEW1) {
            messages_mode = MODE_NEW2;
            frame_show(messages_new2_show, messages_new2_hide);
        } else if(messages_mode == MODE_NEW2) {
            frame_show(messages_loading_show, NULL);

            char *number = etk_entry_text_get(number_entry);

            Etk_Textblock *textblock = etk_text_view_textblock_get(content_text_view);
            char *content = etk_string_get(etk_textblock_text_get(textblock, ETK_FALSE));

            g_debug("Sending to %s:\n%s", number, content);

            // TOD: Add setting to enable SMS reports
            GHashTable *properties;
            properties = g_hash_table_new(NULL, NULL);
            ogsmd_sms_send_message(strdup(number), strdup(content), properties, send_message_callback, NULL);
        }
    } else {
        g_error("Unknown input");
    }
}


void process_message(GValueArray *message) {
    int *id = g_slice_alloc(sizeof(int));
    *id = g_value_get_int(g_value_array_get_nth(message, 0));
    GHashTable *parameters = g_hash_table_new(NULL, NULL);
    g_hash_table_insert(parameters, strdup("number"), strdup(g_value_get_string(g_value_array_get_nth(message, 2))));
    g_hash_table_insert(parameters, strdup("content"), strdup(g_value_get_string(g_value_array_get_nth(message, 3))));
    Etk_Tree_Row *row = etk_tree_row_append(ETK_TREE(tree), NULL, col1, parameters, NULL);
    etk_tree_row_data_set(row, id);
}


void retrieve_messagebook_callback(GError*error, GPtrArray*messages, gpointer userdata) {
    g_debug("retrieve messagebook callback");
    tmp_messages = messages;
    pipe_write(pipe_handler, messages_event, EVENT_LIST_CACHED);
}


void retrieve_message_callback(GError *error, char *status, char *number, char *content, GHashTable *properties, gpointer data) {
    g_debug("STATUS: %s", status);
    g_debug("NUMBER: %s", number);
    g_debug("CONTENT: %s", content);
    tmp_status = strdup(status);
    tmp_number = strdup(number);
    tmp_content = strdup(content);

    // FIXME: Does not work
    //tmp_date = strdup(g_hash_table_lookup(properties, "timestamp"));

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
        window_create("Messages", messages_input, messages_event, NULL);
        ecore_evas_show(ee);
    } else if(event == EVENT_MODE_FOLDERS) {
        edje_object_file_set(edje, UI_FILE, "loading");
        ogsmd_sim_retrieve_messagebook("read", retrieve_messagebook_callback, NULL);
    } else if(event == EVENT_MODE_MESSAGE) {
        frame_show(messages_message_show, NULL);
    } else if(event == EVENT_MODE_LIST) {
        edje_object_file_set(edje, UI_FILE, "loading");
        ogsmd_sim_retrieve_messagebook("read", retrieve_messagebook_callback, NULL);
    } else if(event == EVENT_LIST_CACHED) {
        if(messages_mode == MODE_FOLDERS) {
            frame_show(messages_folders_show, messages_folders_hide);
        } else {
            frame_show(messages_list_show, messages_list_hide);
        }
    } else if(event == EVENT_HIDE) {
        ecore_evas_hide(ee);
    } else {
        g_error("Unknown event %d", event);
    }
}


void messages_folders_show() {
    edje_object_file_set(edje, UI_FILE, "folders");

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
    char label[32];
    sprintf(label, "%d messages", tmp_messages->len);
    GHashTable *parameters = g_hash_table_new(NULL, NULL);
    g_hash_table_insert(parameters, strdup("name"), strdup("Inbox"));
    g_hash_table_insert(parameters, strdup("info"), strdup(label));
    Etk_Tree_Row *row = etk_tree_row_append(ETK_TREE(tree), NULL, col1, parameters, NULL);
    etk_tree_row_data_set(row, strdup("Inbox"));

    // Example entry outbox
    /*
    parameters = g_hash_table_new(NULL, NULL);
    g_hash_table_insert(parameters, strdup("name"), strdup("Outbox"));
    g_hash_table_insert(parameters, strdup("info"), strdup("0 messages"));
    row = etk_tree_row_append(ETK_TREE(tree), NULL, col1, parameters, NULL);
    etk_tree_row_data_set(row, strdup("Outbox"));
    */


    container = etk_embed_new(evas);
    etk_container_add(ETK_CONTAINER(container), tree);
    etk_widget_show_all(container);
    edje_object_part_swallow(edje, "swallow", etk_embed_object_get(ETK_EMBED(container)));
}

void messages_folders_hide() {
    edje_object_part_unswallow(edje, container);
    etk_widget_hide_all(container);
}

void messages_list_show() {
    edje_object_file_set(edje, UI_FILE, "list");
    edje_object_part_text_set(edje, "title", "Inbox"); 

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

    g_ptr_array_foreach(tmp_messages, process_message, NULL);

    container = etk_embed_new(evas);
    etk_container_add(ETK_CONTAINER(container), tree);
    etk_widget_show_all(container);

    edje_object_part_swallow(edje, "swallow", etk_embed_object_get(ETK_EMBED(container)));
}

void messages_list_hide() {
    edje_object_part_unswallow(edje, container);
    etk_widget_hide_all(container);
}

void messages_message_show() {
    edje_object_file_set(edje, UI_FILE, "message_show");
    edje_object_part_text_set(edje, "message_show_status", tmp_status);
    edje_object_part_text_set(edje, "message_show_number", tmp_number);
    edje_object_part_text_set(edje, "message_show_content", tmp_content);
    edje_object_part_text_set(edje, "message_show_date", tmp_date);
}

void messages_loading_show() {
    edje_object_file_set(edje, UI_FILE, "loading");
}

void messages_new1_show() {
    edje_object_part_unswallow(edje, container);
    etk_widget_hide_all(container);

    edje_object_file_set(edje, UI_FILE, "content_edit");

    content_text_view = etk_text_view_new();
    container_content = etk_embed_new(evas);
    etk_container_add(ETK_CONTAINER(container_content), content_text_view);
    etk_widget_show_all(container_content);
    edje_object_part_swallow(edje, "content", etk_embed_object_get(ETK_EMBED(container_content)));


    kbd_show(KEYBOARD_ALPHA);
}

void messages_new1_hide() {
    edje_object_part_unswallow(edje, container_content);
    etk_widget_hide_all(container_content);

    kbd_hide();
}


void messages_new2_show() {
    edje_object_file_set(edje, UI_FILE, "number_edit");

    number_entry = etk_entry_new();
    container_number = etk_embed_new(evas);
    etk_container_add(ETK_CONTAINER(container_number), number_entry);
    etk_widget_show_all(container_number);
    edje_object_part_swallow(edje, "number", etk_embed_object_get(ETK_EMBED(container_number)));

    kbd_show(KEYBOARD_NUMERIC);
}

void messages_new2_hide() {
    edje_object_part_unswallow(edje, container_number);
    etk_widget_hide_all(container_number);

    kbd_hide();
}

void messages_delete_show() {
    edje_object_file_set(edje, UI_FILE, "delete");
}

