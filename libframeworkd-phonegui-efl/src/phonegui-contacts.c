#include "phonegui-contacts.h"
#include <pthread.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <Edje.h>
#include <etk/Etk.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-call.h>
#include "phonegui-init.h"
#include "etk_tree_model_edje.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/contacts.edj"

typedef enum {
    MODE_LIST
} ContactsModes;

typedef enum {
    EVENT_SHOW,
    EVENT_MODE_LIST,
    EVENT_HIDE
} ContactsEvents;


static ContactsModes contacts_mode = MODE_LIST;

static Etk_Widget *container;
static Etk_Widget *tree;
static Etk_Tree_Col *col1;

static GPtrArray *tmp_entries;


void phonegui_contacts_show() {
    g_debug("phonegui_contacts_show()");
    phonegui_input_callback = contacts_input;
    phonegui_event_callback = contacts_event;

    pipe_write(pipe_handler, EVENT_SHOW);
}

void phonegui_contacts_hide() {
}



void contacts_input(void *data, Evas_Object *obj, const char *emission, const char *source) {
    g_debug("contacts_input() input: %s", emission);

    if(!strcmp(emission, "options")) {

        Etk_Tree_Row *row = etk_tree_selected_row_get(tree);
        char *number = etk_tree_row_data_get(row);

        // call
        g_debug("call %s", number);
        ogsmd_call_initiate(number, "voice", NULL, NULL);
      
    } else {
        g_error("Unknown input");
    }
}


void contacts_event(int event) {
    g_debug("contacts_event()");
    g_debug("Event: %d", event);

    if(event == EVENT_SHOW) {
        g_debug("show");

        // Init ETK
        char **empty = NULL;
        etk_init(0, empty);

        edje_object_file_set(edje, UI_FILE, "list");
        ecore_evas_show(ee);

        // Get phonebook
        ogsmd_sim_retrieve_phonebook("contacts", retrieve_phonebook_callback, NULL);

    } else if(event == EVENT_MODE_LIST) {

        tree = etk_tree_new();
        etk_tree_rows_height_set(ETK_TREE(tree), 180);
        etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_LIST);
        etk_tree_headers_visible_set(ETK_TREE(tree), ETK_FALSE);
        etk_tree_multiple_select_set(ETK_TREE(tree), ETK_FALSE);

        col1 = etk_tree_col_new(ETK_TREE(tree), "Title", 300, 0.0);
        etk_tree_col_model_add(col1, etk_tree_model_edje_new(UI_FILE, "row"));
        etk_tree_build(ETK_TREE(tree));

        Etk_Scrolled_View *scrolled_view = etk_tree_scrolled_view_get(ETK_TREE(tree));
        etk_scrolled_view_dragable_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_TRUE);
        //etk_scrolled_view_drag_damping_set(ETK_SCROLLED_VIEW(scrolled_view), 500);
        etk_scrolled_view_drag_bouncy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_FALSE);
        etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_POLICY_SHOW, ETK_POLICY_SHOW);


        // Sort entries
        g_debug("sorting");
        g_ptr_array_sort(tmp_entries, compare_entries);

        // Process entries
        g_debug("processing");
        g_ptr_array_foreach(tmp_entries, process_entry, NULL);


        container = etk_embed_new(evas);
        etk_container_add(ETK_CONTAINER(container), tree);
        etk_widget_show_all(container);
        edje_object_part_swallow(edje, "swallow", etk_embed_object_get(ETK_EMBED(container)));
        g_debug("SWALLOW SET!");

    } else if(event == EVENT_HIDE) {
        g_debug("hide");
        ecore_evas_hide(ee);
    } else {
        g_error("Unknown event %d", event);
    }
}


void retrieve_phonebook_callback(GError *error, GPtrArray *entries, gpointer userdata) {
    g_debug("retrieve phonebook callback");
    tmp_entries = entries;
    pipe_write(pipe_handler, EVENT_MODE_LIST);
}

gint compare_entries(GValueArray **a, GValueArray **b) {
    return strcmp(
        g_value_get_string(g_value_array_get_nth(*a, 1)),
        g_value_get_string(g_value_array_get_nth(*b, 1))
    );
}

void process_entry(GValueArray *entry) {
    g_debug("process_entry()");

    int *id = g_slice_alloc(sizeof(int));
    *id = g_value_get_int(g_value_array_get_nth(entry, 0));

    GHashTable *parameters = g_hash_table_new(NULL, NULL);
    g_hash_table_insert(parameters, strdup("name"), strdup(g_value_get_string(g_value_array_get_nth(entry, 1))));
    g_hash_table_insert(parameters, strdup("number"), strdup(g_value_get_string(g_value_array_get_nth(entry, 2))));

    Etk_Tree_Row *row = etk_tree_row_append(ETK_TREE(tree), NULL, col1, parameters, NULL);

    // TODO: Use id instead of number!
    etk_tree_row_data_set(row, strdup(g_value_get_string(g_value_array_get_nth(entry, 2))));
}

