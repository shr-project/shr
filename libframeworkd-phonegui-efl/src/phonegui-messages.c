#include "phonegui-messages.h"
#include <pthread.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <Edje.h>
/* #include <Etk.h> */
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>
#include "pipe.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/messages.edj"

#define EVENT_MODE_MAIN "a"
#define EVENT_MODE_INBOX "b"
#define EVENT_MODE_OUTBOX "c"

static PipeHandler pipe_handler;

static Ecore_Evas   *ee;
static Evas         *evas;
static Evas_Object  *edje;
static double       edje_w, edje_h;

static char number[64];
static int  number_length = 0;

static MessagesMode messages_mode = MODE_MAIN;


void phonegui_messages_launch(int argc, char** argv) {
    g_debug("phonegui_messages_launch()");

    /* Create pipe */
    pipe_handler = pipe_create();
    g_debug("pipe created");

    ecore_init();
    ecore_evas_init();
    ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
    ecore_evas_title_set(ee, "openmoko-messages3");
    ecore_evas_borderless_set(ee, 0);
    ecore_evas_shaped_set(ee, 0);

    evas = ecore_evas_get(ee);
    evas_font_path_append(evas, "/usr/local/share/edje/data/test/fonts/");

    edje_init();
    edje = edje_object_add(evas);
    edje_object_file_set(edje, UI_FILE, "main");
    evas_object_move(edje, 0, 0);
    //edje_object_size_min_get(edje, &edje_w, &edje_h);
    evas_object_resize(edje, 480, 600);
    evas_object_show(edje);
    ecore_evas_resize(ee, (int)edje_w, (int)edje_h);
    ecore_evas_show(ee);

    edje_object_signal_callback_add(edje, "*", "input", messages_ui_input, "data");

    Ecore_Fd_Handler *handler = ecore_main_fd_handler_add(pipe_handler.input, ECORE_FD_READ, messages_ui_event, NULL, NULL, NULL);
    ecore_main_fd_handler_active_set(handler, ECORE_FD_READ);

    /* etk_init(argc, argv); */

    ecore_main_loop_begin();
}

void messages_ui_input(void *data, Evas_Object *obj, const char *emission, const char *source) {
    g_debug("messages_ui_input() input: %s", emission);

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


void messages_ui_event(void *data, Ecore_Fd_Handler *fdh) {
    g_debug("messages_ui_event()");

    char* event = pipe_read(pipe_handler);
    g_debug("event: %s", event);

    if(!strcmp(event, EVENT_MODE_INBOX))
    {
        g_debug("inbox");
        edje_object_file_set(edje, UI_FILE, "list");
        edje_object_part_text_set(edje, "title", "Inbox"); 
        ogsmd_sim_retrieve_messagebook("read", retrieve_messagebook_callback, NULL);
        messages_mode = MODE_INBOX;
    }
    else if(!strcmp(event, EVENT_MODE_OUTBOX))
    {
        g_debug("outbox");
        edje_object_file_set(edje, UI_FILE, "list");
        edje_object_part_text_set(edje, "title", "Outbox"); 
        messages_mode = MODE_OUTBOX;
    }
    else
    {
        g_error("Unknown event");
    }
}

void messages_ui_update() {
    g_debug("messages_ui_update()");
    edje_object_part_text_set(edje, "number", number);
}

void messages_call_initiate_callback(GError *error, int call_id, gpointer userdata) {
    g_debug("call_initiate_callback()");

    /* Exit */
    ecore_main_loop_quit();
}


