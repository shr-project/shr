#include "phonegui-init.h"
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <etk/Etk.h>
#include <Elementary.h>
#include <pthread.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>


void ui_init() {


    /*
     * Initializations
     */

    elm_init(phonegui_argc, phonegui_argv);
    g_debug("Initiated elementary");

    etk_init(phonegui_argc, phonegui_argv);
    g_debug("Initiated etk");

    // Add ecore exit callback
    if(phonegui_exit_cb != NULL) {
        g_debug("Added exit callback to ecore.");
        ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, phonegui_exit_cb, NULL);
    }

    // Add ecore pipe handler
    Ecore_Fd_Handler *handler = ecore_main_fd_handler_add(pipe_handler.input, ECORE_FD_READ, event_callback, NULL, NULL, NULL);
    ecore_main_fd_handler_active_set(handler, ECORE_FD_READ);    

    elm_run();
}

void phonegui_init(int argc, char **argv, void (*exit_cb)()) {
    g_debug("phonegui_init()");

    // Assign arguments
    phonegui_argc = argc;
    phonegui_argv = argv;
    phonegui_exit_cb = exit_cb;

    // Create pipe
    pipe_handler = pipe_create();



    // Create thread for ecore loop
    g_debug("Entering ecore loop");
    pthread_t thread;
    int rc = pthread_create(&thread, NULL, (void*) ui_init, NULL);
    if(rc) {
        g_error("Return code from pthread_create() is %d", rc);
    }
}


void window_create(
    char *title,
    void (*event_cb)(int event),
    void (*delete_cb)(Ecore_Evas *ecore_evas)
) {
    g_debug("window_create()");

    // Window
    win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
    elm_win_title_set(win, title);
    elm_win_autodel_set(win, 0); // Enable it?
    if(delete_cb != NULL) {
        g_debug("Adding delete-request-callback");
        evas_object_smart_callback_add(win, "delete-request", delete_cb, NULL);
    }

    // Background
    bg = elm_bg_add(win);
    evas_object_size_hint_weight_set(bg, 1.0, 1.0);
    elm_win_resize_object_add(win, bg);
    evas_object_show(bg);

    // Layout
    layout = elm_layout_add(win);
    elm_win_resize_object_add(win, layout);
    evas_object_show(layout);

    evas_object_resize(win, 320, 520);
    evas_object_show(win);
}

void window_destroy() {
    evas_object_del(layout);
    evas_object_del(bg);
    evas_object_del(win);
}

int event_callback(void *data, Ecore_Fd_Handler *fdh) {
    g_debug("event_callback()");
    PipeMessage *m;
    while((m = pipe_read(pipe_handler)) != -1) {
        if(m->callback != NULL) {
            m->callback(m->event); 
        }
    }
    return 1;
}

