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
#include "frame.h"
#include "kbd.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/contacts.edj"

typedef enum {
    MODE_NEW,
    MODE_MODIFY,
    MODE_LIST,
    MODE_OPTIONS
} ContactsModes;

typedef enum {
    EVENT_SHOW,
    EVENT_MODE_LIST,
    EVENT_MODE_LIST_CACHED,
    EVENT_NEW_SAVE,
    EVENT_HIDE
} ContactsEvents;


static ContactsModes contacts_mode;

static Etk_Widget *container;
static Etk_Widget *tree;
static Etk_Tree_Col *col1;

static Etk_Widget *entry_number;
static Etk_Widget *entry_name;
static Etk_Widget *container_number;
static Etk_Widget *container_name;

static GPtrArray *tmp_entries;
static GValueArray *tmp_entry;

static char *tmp_name;
static char *tmp_number;

static int free_entry_index = -1;

static Evas_Object *bt1, *bt2, *bt3, *bt4;


void phonegui_contacts_show() {
    g_debug("phonegui_contacts_show()");

    contacts_mode = MODE_LIST;
    pipe_write(pipe_handler, contacts_event, EVENT_SHOW);
    pipe_write(pipe_handler, contacts_event, EVENT_MODE_LIST);
}

void phonegui_contacts_hide() {
    pipe_write(pipe_handler, contacts_event, EVENT_HIDE);
}

void contacts_delete(void *data, Evas_Object *obj, void *event_info) {
    phonegui_exit_cb();
}



void contacts_input(void *data, Evas_Object *obj, const char *emission, const char *source) {
    g_debug("contacts_input() input: %s", emission);
}



void contacts_event(int event) {
    g_debug("contacts_event(), event: %d", event);

    if(event == EVENT_SHOW) {
        window_create("Contacts", contacts_input, contacts_event, contacts_delete);
    } else if(event == EVENT_MODE_LIST) {
        frame_show(contacts_loading_show, NULL);
        evas_object_show(win);
        ogsmd_sim_retrieve_phonebook("contacts", retrieve_phonebook_callback, NULL);
    } else if(event == EVENT_MODE_LIST_CACHED) {
        frame_show(contacts_list_show, contacts_list_hide);
    } else if(event == EVENT_NEW_SAVE) {
        ogsmd_sim_store_entry(
            "contacts",
            free_entry_index,
            etk_entry_text_get(entry_name),
            etk_entry_text_get(entry_number),
            NULL,
            NULL
        );
        free_entry_index = -1;

        frame_show(contacts_loading_show, NULL);
        ogsmd_sim_retrieve_phonebook("contacts", retrieve_phonebook_callback, NULL);
    } else if(event == EVENT_HIDE) {
        evas_object_hide(win);
    } else {
        g_error("Unknown event %d", event);
    }
}


void retrieve_phonebook_callback(GError *error, GPtrArray *entries, gpointer userdata) {
    g_debug("retrieve phonebook callback");
    tmp_entries = entries;
    pipe_write(pipe_handler, contacts_event, EVENT_MODE_LIST_CACHED);
}


int get_phonebook_entry_id(GValueArray *entry) {
    g_debug("  second: %d", g_value_get_int(g_value_array_get_nth(entry, 0)));
    return g_value_get_int(g_value_array_get_nth(entry, 0));
}


void add_entry_index(GValueArray *entry, int* indizes) {
    int i = 0;
    while(indizes[i] != -1)
        i++;

    int id = g_value_get_int(g_value_array_get_nth(entry, 0));
    indizes[i] = id;
}


int calculate_free_entry_index(int min, int max, GPtrArray *entries) {

    int *indizes = g_slice_alloc(sizeof(int) * entries->len);
    memset(indizes, -1, sizeof(int) * entries->len);
    g_ptr_array_foreach(entries, add_entry_index, indizes);
    

    // TODO: Optimize the loops
    int i, j;
    g_debug("length: %d", tmp_entries->len);

    for(i = min ; i <= max && free_entry_index == -1 ; i++) {
        gboolean found = FALSE;
        for(j = 0 ; j < tmp_entries->len && found == FALSE ; j++) {
            if(i == indizes[j]) {
                found = TRUE;
            }
        }
        if(found == FALSE) {
            free_entry_index = i;
            g_debug("Found free entry: %d", i);
        }
    }

    if(free_entry_index == -1) {
        g_error("No free entry index found. SIM probably full!");
    }
}


void get_phonebook_info_callback(GError *error, GHashTable *info, gpointer userdata) {
    int *min = g_hash_table_lookup(info, "min_index");
    int *max = g_hash_table_lookup(info, "max_index");
    // FIXME: Min and max is always 24, but why? Work around this bug!

    free_entry_index = calculate_free_entry_index(1, 150, tmp_entries);

    pipe_write(pipe_handler, contacts_event, EVENT_NEW_SAVE);
}

gint compare_entries(GValueArray **a, GValueArray **b) {
    return strcasecmp(
        g_value_get_string(g_value_array_get_nth(*a, 1)),
        g_value_get_string(g_value_array_get_nth(*b, 1))
    );
}

void process_entry(GValueArray *entry, gpointer userdata) {
    g_debug("process_entry()");

    GHashTable *parameters = g_hash_table_new(NULL, NULL);
    g_hash_table_insert(parameters, strdup("name"), strdup(g_value_get_string(g_value_array_get_nth(entry, 1))));
    g_hash_table_insert(parameters, strdup("number"), strdup(g_value_get_string(g_value_array_get_nth(entry, 2))));
    Etk_Tree_Row *row = etk_tree_row_append(ETK_TREE(tree), NULL, col1, parameters, NULL);

    etk_tree_row_data_set(row, g_value_array_copy(entry));
}



/*
 * Button triggers
 */

void contacts_button_new_clicked() {
    contacts_mode = MODE_NEW;
    frame_show(contacts_new_show, contacts_new_hide);
}

void contacts_button_call_clicked() {
    if(contacts_mode == MODE_LIST) {
        Etk_Tree_Row *row = etk_tree_selected_row_get(tree);
        if(row != NULL) {
            GValueArray *data = etk_tree_row_data_get(row);
            char *number = g_value_get_string(g_value_array_get_nth(data, 2));
            ogsmd_call_initiate(number, "voice", NULL, NULL);
        }
    } else if(contacts_mode == MODE_OPTIONS) {
        char *number = g_value_get_string(g_value_array_get_nth(tmp_entry, 2));
        ogsmd_call_initiate(number, "voice", NULL, NULL);
    }
}

void contacts_button_options_clicked() {
    Etk_Tree_Row *row = etk_tree_selected_row_get(tree);
    if(row != NULL) {
        contacts_mode = MODE_OPTIONS;
        tmp_entry = etk_tree_row_data_get(row);
        frame_show(contacts_options_show, contacts_options_hide);
    }
}

void contacts_button_save_clicked() {
    if(strcmp(etk_entry_text_get(entry_name), "") && strcmp(etk_entry_text_get(entry_number), "")) {
        if(contacts_mode == MODE_NEW) {
            ogsmd_sim_get_phonebook_info("contacts", get_phonebook_info_callback, NULL);
        } else if(contacts_mode == MODE_MODIFY) {
            ogsmd_sim_store_entry(
                "contacts",
                g_value_get_int(g_value_array_get_nth(tmp_entry, 0)),
                etk_entry_text_get(entry_name),
                etk_entry_text_get(entry_number),
                NULL,
                NULL
            );
        }
        g_debug("SAVED");
        pipe_write(pipe_handler, contacts_event, EVENT_MODE_LIST);
    }
}

void contacts_button_back_clicked() {
    contacts_mode = MODE_LIST;
    frame_show(contacts_list_show, contacts_list_hide);
}

void contacts_button_modify_clicked() {
    contacts_mode = MODE_MODIFY;
    frame_show(contacts_modify_show, contacts_modify_hide);
}

void contacts_button_delete_clicked() {
    frame_show(contacts_delete_show, contacts_delete_hide);
}

void contacts_button_delete_yes_clicked() {
    ogsmd_sim_delete_entry(
        "contacts",
        g_value_get_int(g_value_array_get_nth(tmp_entry, 0)),
        NULL,
        NULL
    );
    pipe_write(pipe_handler, contacts_event, EVENT_MODE_LIST);
}

void contacts_button_delete_no_clicked() {
    pipe_write(pipe_handler, contacts_event, EVENT_MODE_LIST_CACHED);
}


/*
 * Views
 */

void contacts_list_show() {
    elm_layout_file_set(layout, UI_FILE, "list");

    bt1 = elm_button_add(win);
    elm_button_label_set(bt1, "New");
    evas_object_smart_callback_add(bt1, "clicked", contacts_button_new_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_new", bt1);
    evas_object_show(bt1);

    bt2 = elm_button_add(win);
    elm_button_label_set(bt2, "Call");
    evas_object_smart_callback_add(bt2, "clicked", contacts_button_call_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_call", bt2);
    evas_object_show(bt2);

    bt3 = elm_button_add(win);
    elm_button_label_set(bt3, "Options");
    evas_object_smart_callback_add(bt3, "clicked", contacts_button_options_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_options", bt3);
    evas_object_show(bt3);

    
    tree = etk_tree_new();
    etk_tree_rows_height_set(ETK_TREE(tree), 80);
    etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_LIST);
    etk_tree_headers_visible_set(ETK_TREE(tree), ETK_FALSE);
    etk_tree_multiple_select_set(ETK_TREE(tree), ETK_FALSE);

    col1 = etk_tree_col_new(ETK_TREE(tree), "Title", 300, 0.0);
    etk_tree_col_model_add(col1, etk_tree_model_edje_new(UI_FILE, "row"));
    etk_tree_build(ETK_TREE(tree));

    Etk_Scrolled_View *scrolled_view = etk_tree_scrolled_view_get(ETK_TREE(tree));
    etk_scrolled_view_dragable_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_TRUE);
    etk_scrolled_view_drag_damping_set(ETK_SCROLLED_VIEW(scrolled_view), 400);
    etk_scrolled_view_drag_bouncy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_FALSE);
    //etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_POLICY_HIDE, ETK_POLICY_HIDE);

    // Sort entries
    g_ptr_array_sort(tmp_entries, compare_entries);

    // Process entries
    g_ptr_array_foreach(tmp_entries, process_entry, NULL);

    container = etk_embed_new(evas_object_evas_get(win));
    etk_container_add(ETK_CONTAINER(container), tree);
    etk_widget_show_all(container);
    edje_object_part_swallow(elm_layout_edje_get(layout), "swallow", etk_embed_object_get(ETK_EMBED(container)));

    g_debug("Swallowed!");
}

void contacts_list_hide() {
    edje_object_part_unswallow(elm_layout_edje_get(layout), bt1);
    evas_object_del(bt1);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt2);
    evas_object_del(bt2);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt3);
    evas_object_del(bt3);

    edje_object_part_unswallow(elm_layout_edje_get(layout), container);
    etk_widget_hide_all(container);
}

void contacts_options_show() {
    elm_layout_file_set(layout, UI_FILE, "options");
    edje_object_part_text_set(elm_layout_edje_get(layout), "name", g_value_get_string(g_value_array_get_nth(tmp_entry, 1)));
    edje_object_part_text_set(elm_layout_edje_get(layout), "number", g_value_get_string(g_value_array_get_nth(tmp_entry, 2)));

    bt1 = elm_button_add(win);
    elm_button_label_set(bt1, "Back");
    evas_object_smart_callback_add(bt1, "clicked", contacts_button_back_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_back", bt1);
    evas_object_show(bt1);

    bt2 = elm_button_add(win);
    elm_button_label_set(bt2, "Call");
    evas_object_smart_callback_add(bt2, "clicked", contacts_button_call_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_call", bt2);
    evas_object_show(bt2);

    bt3 = elm_button_add(win);
    elm_button_label_set(bt3, "Modify");
    evas_object_smart_callback_add(bt3, "clicked", contacts_button_modify_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_modify", bt3);
    evas_object_show(bt3);

    bt4 = elm_button_add(win);
    elm_button_label_set(bt4, "Delete");
    evas_object_smart_callback_add(bt4, "clicked", contacts_button_delete_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_delete", bt4);
    evas_object_show(bt4);
}

void contacts_options_hide() {
    edje_object_part_unswallow(elm_layout_edje_get(layout), bt1);
    evas_object_del(bt1);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt2);
    evas_object_del(bt2);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt3);
    evas_object_del(bt3);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt4);
    evas_object_del(bt4);
}

void contacts_loading_show() {
    elm_layout_file_set(layout, UI_FILE, "loading");
}

void contacts_modify_show() {
    contacts_new_show();

    elm_layout_file_set(layout, UI_FILE, "modify");
    edje_object_part_text_set(elm_layout_edje_get(layout), "title", "Modify Contact");

    etk_entry_text_set(entry_number, g_value_get_string(g_value_array_get_nth(tmp_entry, 2)));
    etk_entry_text_set(entry_name, g_value_get_string(g_value_array_get_nth(tmp_entry, 1)));
}

void contacts_modify_hide() {
    contacts_new_hide();
}

void contacts_new_show() {
    kbd_show(KEYBOARD_ALPHA);

    elm_layout_file_set(layout, UI_FILE, "modify");
    edje_object_part_text_set(elm_layout_edje_get(layout), "title", "New Contact");

    bt1 = elm_button_add(win);
    elm_button_label_set(bt1, "Back");
    evas_object_smart_callback_add(bt1, "clicked", contacts_button_back_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_back", bt1);
    evas_object_show(bt1);

    bt2 = elm_button_add(win);
    elm_button_label_set(bt2, "Save");
    evas_object_smart_callback_add(bt2, "clicked", contacts_button_save_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_save", bt2);
    evas_object_show(bt2);

    entry_number = etk_entry_new();
    container_number = etk_embed_new(evas_object_evas_get(win));
    etk_container_add(ETK_CONTAINER(container_number), entry_number);
    etk_widget_show_all(container_number);
    edje_object_part_swallow(elm_layout_edje_get(layout), "number", etk_embed_object_get(ETK_EMBED(container_number)));

    entry_name = etk_entry_new();
    container_name = etk_embed_new(evas_object_evas_get(win));
    etk_container_add(ETK_CONTAINER(container_name), entry_name);
    etk_widget_show_all(container_name);
    edje_object_part_swallow(elm_layout_edje_get(layout), "name", etk_embed_object_get(ETK_EMBED(container_name)));
}

void contacts_new_hide() {
    edje_object_part_unswallow(elm_layout_edje_get(layout), bt1);
    evas_object_del(bt1);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt2);
    evas_object_del(bt2);

    edje_object_part_unswallow(elm_layout_edje_get(layout), container_number);
    etk_widget_hide_all(container_number);

    edje_object_part_unswallow(elm_layout_edje_get(layout), container_name);
    etk_widget_hide_all(container_name);

    kbd_hide();
}

void contacts_delete_show() {
    elm_layout_file_set(layout, UI_FILE, "delete");

    bt1 = elm_button_add(win);
    elm_button_label_set(bt1, "Yes");
    evas_object_smart_callback_add(bt1, "clicked", contacts_button_delete_yes_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_yes", bt1);
    evas_object_show(bt1);

    bt2 = elm_button_add(win);
    elm_button_label_set(bt2, "No");
    evas_object_smart_callback_add(bt2, "clicked", contacts_button_delete_no_clicked, NULL);
    edje_object_part_swallow(elm_layout_edje_get(layout), "button_no", bt2);
    evas_object_show(bt2);
}

void contacts_delete_hide() {
    edje_object_part_unswallow(elm_layout_edje_get(layout), bt1);
    evas_object_del(bt1);

    edje_object_part_unswallow(elm_layout_edje_get(layout), bt2);
    evas_object_del(bt2);
}

