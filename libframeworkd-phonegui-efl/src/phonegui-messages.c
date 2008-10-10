#include "phonegui-messages.h"
#include <pthread.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <Edje.h>
#include <etk/Etk.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>
#include "phonegui-init.h"
#include "etk_tree_model_edje.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/messages.edj"

typedef enum {
    MODE_FOLDERS,
    MODE_LIST,
    MODE_MESSAGE
} MessagesModes;

typedef enum {
    EVENT_SHOW,
    EVENT_MODE_FOLDERS,
    EVENT_MODE_LIST,
    EVENT_MODE_LIST_CACHED,
    EVENT_MODE_MESSAGE,
    EVENT_HIDE
} MessagesEvents;


static MessagesModes messages_mode;

static Etk_Widget *container = NULL;
static Etk_Widget *tree;
static Etk_Tree_Col *col1;

static char *messages_category;

// TODO: Remove tmp variables
static GPtrArray *tmp_messages;
static char *tmp_number;
static char *tmp_content;
static char *tmp_status;

void phonegui_messages_show(int argc, char** argv) {
    g_debug("phonegui_messages_show()");
    phonegui_input_callback = messages_input;
    phonegui_event_callback = messages_event;

    messages_mode = MODE_FOLDERS;
    pipe_write(pipe_handler, EVENT_MODE_FOLDERS);
    pipe_write(pipe_handler, EVENT_SHOW);
}

void phonegui_messages_hide() {
}



void messages_input(void *data, Evas_Object *obj, const char *emission, const char *source) {
    g_debug("messages_input() input: %s", emission);

    if(!strcmp(emission, "options")) {
        g_debug("options");
    } else if(!strcmp(emission, "show")) {
        if(messages_mode == MODE_FOLDERS) {
            messages_mode = MODE_LIST;
            Etk_Tree_Row *row = etk_tree_selected_row_get(tree);
            if(row != NULL) {
                edje_object_part_unswallow(edje, container);
                etk_widget_hide_all(container);
                edje_object_file_set(edje, UI_FILE, "loading");

                messages_category = etk_tree_row_data_get(row);
                pipe_write(pipe_handler, EVENT_MODE_LIST);
            }
        } else if(messages_mode == MODE_LIST) {
            messages_mode = MODE_MESSAGE;
            Etk_Tree_Row *row = etk_tree_selected_row_get(tree);
            if(row != NULL) {
                edje_object_part_unswallow(edje, container);
                etk_widget_hide_all(container);
                edje_object_file_set(edje, UI_FILE, "loading");

                int *id = etk_tree_row_data_get(row);
                ogsmd_sim_retrieve_message(*id, retrieve_message_callback, NULL);
            }
        }
    } else if(!strcmp(emission, "back")) {
        edje_object_part_unswallow(edje, container);
        etk_widget_hide_all(container);

        if(messages_mode == MODE_MESSAGE) {
            messages_mode = MODE_LIST;
            pipe_write(pipe_handler, EVENT_MODE_LIST_CACHED);
        } else if(messages_mode == MODE_LIST) {
            messages_mode = MODE_FOLDERS;
            pipe_write(pipe_handler, EVENT_MODE_FOLDERS);
        }
    } else if(!strcmp(emission, "answer")) {
        // TODO
    } else if(!strcmp(emission, "exit")) {
        exit(0);
    } else {
        g_error("Unknown input");
    }
}


void process_message(GValueArray *message) {
    g_debug("G");
    g_debug("H: %d", message);

    g_debug("ID: %s", g_strdup_value_contents(g_value_array_get_nth(message, 0)));
    g_debug("Status: %s", g_strdup_value_contents(g_value_array_get_nth(message, 1)));
    g_debug("Absender: %s", g_strdup_value_contents(g_value_array_get_nth(message, 2)));
    g_debug("Inhalt: %s\n", g_strdup_value_contents(g_value_array_get_nth(message, 3)));
    

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
    pipe_write(pipe_handler, EVENT_MODE_LIST_CACHED);
}


void retrieve_message_callback(GError *error, char *status, char *number, char *content, GHashTable *properties, gpointer data) {
    g_debug("STATUS: %s", status);
    g_debug("NUMBER: %s", number);
    g_debug("CONTENT: %s", content);
    tmp_status = strdup(status);
    tmp_number = strdup(number);
    tmp_content = strdup(content);
    pipe_write(pipe_handler, EVENT_MODE_MESSAGE);
}

void messages_event(int event) {
    g_debug("messages_event()");
    g_debug("Event: %d", event);

    if(event == EVENT_SHOW) {
        g_debug("show");
        ecore_evas_show(ee);
    } else if(event == EVENT_MODE_FOLDERS) {
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
        GHashTable *parameters = g_hash_table_new(NULL, NULL);
        g_hash_table_insert(parameters, strdup("name"), strdup("Inbox"));
        g_hash_table_insert(parameters, strdup("info"), strdup("16 messages"));
        Etk_Tree_Row *row = etk_tree_row_append(ETK_TREE(tree), NULL, col1, parameters, NULL);
        etk_tree_row_data_set(row, strdup("Inbox"));

        // Example entry outbox
        parameters = g_hash_table_new(NULL, NULL);
        g_hash_table_insert(parameters, strdup("name"), strdup("Outbox"));
        g_hash_table_insert(parameters, strdup("info"), strdup("0 messages"));
        row = etk_tree_row_append(ETK_TREE(tree), NULL, col1, parameters, NULL);
        etk_tree_row_data_set(row, strdup("Outbox"));


        container = etk_embed_new(evas);
        etk_container_add(ETK_CONTAINER(container), tree);
        etk_widget_show_all(container);
        edje_object_part_swallow(edje, "swallow", etk_embed_object_get(ETK_EMBED(container)));

    } else if(event == EVENT_MODE_MESSAGE) {
        edje_object_file_set(edje, UI_FILE, "message_show");
        edje_object_part_text_set(edje, "message_show_status", tmp_status);
        edje_object_part_text_set(edje, "message_show_number", tmp_number);
        edje_object_part_text_set(edje, "message_show_content", tmp_content);
    } else if(event == EVENT_MODE_LIST) {
        g_debug("inbox");
        edje_object_file_set(edje, UI_FILE, "loading");

        // this caches the list and emits EVENT_MODE_LIST_CACHED
        ogsmd_sim_retrieve_messagebook("read", retrieve_messagebook_callback, NULL);
    } else if(event == EVENT_MODE_LIST_CACHED) {

        edje_object_file_set(edje, UI_FILE, "list");
        if(!strcmp(messages_category, "inbox")) {
            edje_object_part_text_set(edje, "title", "Inbox"); 
        } else {
            edje_object_part_text_set(edje, "title", "Outbox");     
        }

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
g_debug("5");
        g_ptr_array_foreach(tmp_messages, process_message, NULL);
g_debug("6");
        container = etk_embed_new(evas);
        etk_container_add(ETK_CONTAINER(container), tree);
        etk_widget_show_all(container);

        edje_object_part_swallow(edje, "swallow", etk_embed_object_get(ETK_EMBED(container)));
    } else if(event == EVENT_HIDE) {
        g_debug("hide");
        ecore_evas_hide(ee);
    } else {
        g_error("Unknown event %d", event);
    }
}



