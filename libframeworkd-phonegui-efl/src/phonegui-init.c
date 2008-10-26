#include "phonegui-init.h"
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <etk/Etk.h>
#include <pthread.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>


void phonegui_init(int argc, char **argv, void (*exit_cb)()) {
    g_debug("phonegui_init()");

    // Assign arguments
    phonegui_argc = argc;
    phonegui_argv = argv;
    phonegui_exit_cb = exit_cb;

    // Create pipe
    pipe_handler = pipe_create();


    /*
     * Initializations
     */
    ecore_init();
    g_debug("Initiated ecore");

    ecore_evas_init();
    g_debug("Initiated ecore_evas");

    etk_init(NULL, NULL); //etk_init(phonegui_argc, phonegui_argv);
    g_debug("Initiated etk");

    edje_init();
    g_debug("Initiated edje");


    // Add ecore exit callback
    if(phonegui_exit_cb != NULL) {
        g_debug("Added exit callback to ecore.");
        ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, phonegui_exit_cb, NULL);
    }

    // Add ecore pipe handler
    Ecore_Fd_Handler *handler = ecore_main_fd_handler_add(pipe_handler.input, ECORE_FD_READ, event_callback, NULL, NULL, NULL);
    ecore_main_fd_handler_active_set(handler, ECORE_FD_READ);

    // Create thread for ecore loop
    pthread_t thread;
    int rc = pthread_create(&thread, NULL, (void*) ecore_main_loop_begin, NULL);
    if(rc) {
        g_error("Return code from pthread_create() is %d", rc);
    }
}


void window_create(
    char *title,
    void (*input_cb)(void *data, Evas_Object *obj, const char *emission, const char *source), 
    void (*event_cb)(int event),
    void (*delete_cb)(Ecore_Evas *ecore_evas)
) {
    g_debug("window_create()");

    ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
    if(ee == NULL) {
        g_error("Unable to get x11 convas. Try:\n\nexport DISPLAY=:0.0");
    }

    ecore_evas_title_set(ee, strdup(title));
    ecore_evas_borderless_set(ee, 0);
    ecore_evas_shaped_set(ee, 1);
    ecore_evas_callback_resize_set(ee, window_resize_callback);
    if(delete_cb != NULL) {
        ecore_evas_callback_delete_request_set(ee, delete_cb);
    }

    evas = ecore_evas_get(ee);
    edje = edje_object_add(evas);
    evas_object_move(edje, 0, 0);
    //edje_object_size_min_get(edje, &edje_w, &edje_h);
    evas_object_resize(edje, 480, 600);
    evas_object_show(edje);
    // ecore_evas_resize(ee, (int) edje_w, (int) edje_h);
    edje_object_signal_callback_add(edje, "*", "input", input_cb, NULL);
}

void window_resize_callback(Evas *ev) {
    g_debug("window_resize_callback()");
    int w, h;
    evas_object_move(ee, 0, 0);
    evas_output_size_get(ecore_evas_get(ev), &w, &h);
    evas_object_resize(edje, w, h);
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

