#include "phonegui-messages.h"
#include <pthread.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <Edje.h>
/* #include <Etk.h> */
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>
#include "phonegui-init.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/messages.edj"

typedef enum {
    MODE_MAIN,
    MODE_INBOX,
    MODE_OUTBOX
} MessagesModes;

typedef enum {
    EVENT_SHOW,
    EVENT_MODE_MAIN,
    EVENT_MODE_INBOX,
    EVENT_MODE_OUTBOX,
    EVENT_HIDE
} MessagesEvents;


static MessagesModes messages_mode = MODE_MAIN;
static char number[64];
static int  number_length = 0;


void phonegui_messages_show(int argc, char** argv) {
    /* etk_init(argc, argv); */
    pipe_write(pipe_handler, EVENT_SHOW);
}

void phonegui_messages_hide() {
}

void messages_input(void *data, Evas_Object *obj, const char *emission, const char *source) {
    g_debug("messages_input() input: %s", emission);

    if(!strcmp(emission, "inbox"))
    {
        g_debug("inbox");
        pipe_write(pipe_handler, EVENT_MODE_INBOX);
    }
    else if(!strcmp(emission, "outbox"))
    {
        g_debug("outbox");
        pipe_write(pipe_handler, EVENT_MODE_OUTBOX);
    }
    else
    {
        g_error("Unknown input");
    }
}


void process_message(GValueArray *message) {
    g_debug("PROCESS MESSAGE()");

    int a = g_value_array_get_nth(message, 0);
    g_debug("MESSAGE_ID: %d", a);

//    etk_tree_row_append(ETK_TREE(tree), NULL, col1, data, NULL);
}


void retrieve_messagebook_callback(GError*error, GPtrArray*messages, gpointer userdata) {
    g_debug("retrieve messagebook callback");

/*    Etk_Widget *container = etk_embed_new(evas);

    Etk_Widget *tree = etk_tree_new();
    etk_tree_rows_height_set(ETK_TREE(tree), 50);
    etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_LIST);
    etk_tree_headers_visible_set(ETK_TREE(tree), ETK_FALSE);
    etk_tree_multiple_select_set(ETK_TREE(tree), ETK_FALSE);

    Etk_Tree_Col *col1 = etk_tree_col_new(ETK_TREE(tree), "Title", 300, 0.0);
    g_debug("COL1: %d", col1);
    etk_tree_col_model_add(col1, etk_tree_model_text_new());
    etk_tree_build(ETK_TREE(tree));

    Etk_Scrolled_View *scrolled_view = etk_tree_scrolled_view_get(ETK_TREE(tree));
    etk_scrolled_view_dragable_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_TRUE);
    etk_scrolled_view_drag_bouncy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_FALSE);
*/
    // Process messages
    g_ptr_array_foreach(messages, process_message, NULL);



  /*  etk_container_add(ETK_CONTAINER(container), tree);
    etk_widget_show_all(container);
    edje_object_part_swallow(edje, "swallow", etk_embed_object_get(ETK_EMBED(container)));
    g_debug("SWALLOW SET!");
*/
}


void messages_event(int event) {
    g_debug("messages_event()");
    g_debug("Event: %d", event);

    if(event == EVENT_SHOW) {
        g_debug("show");
        edje_object_file_set(edje, UI_FILE, "main");
        ecore_evas_show(ee);
    } else if(event == EVENT_MODE_INBOX) {
        g_debug("inbox");
        edje_object_file_set(edje, UI_FILE, "list");
        edje_object_part_text_set(edje, "title", "Inbox"); 
        ogsmd_sim_retrieve_messagebook("read", retrieve_messagebook_callback, NULL);
        messages_mode = MODE_INBOX;
    } else if(event == EVENT_MODE_OUTBOX) {
        g_debug("outbox");
        edje_object_file_set(edje, UI_FILE, "list");
        edje_object_part_text_set(edje, "title", "Outbox"); 
        messages_mode = MODE_OUTBOX;
    } else if(event == EVENT_HIDE) {
        g_debug("hide");
        ecore_evas_hide(ee);
    } else {
        g_error("Unknown event %d", event);
    }
}



