#include "phonegui-init.h"
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <pthread.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>


void phonegui_init(int argc, char **argv) {
    g_debug("phonegui_init()");

    pthread_t thread;
    int rc;

    // Create pipe
    pipe_handler = pipe_create();

    // Create thread
    rc = pthread_create(&thread, NULL, (void*) ui_init, NULL);
    if(rc) {
        g_error("Return code from pthread_create() is %d", rc);
    }
}

void ui_init() {
    g_debug("ui_init()");

    ecore_init();
    ecore_evas_init();
    ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
    ecore_evas_title_set(ee, "phonegui");
    ecore_evas_borderless_set(ee, 0);
    ecore_evas_shaped_set(ee, 0);

    evas = ecore_evas_get(ee);

    edje_init();
    edje = edje_object_add(evas);
    evas_object_move(edje, 0, 0);
    //edje_object_size_min_get(edje, &edje_w, &edje_h);
    evas_object_resize(edje, 480, 600);
    evas_object_show(edje);
    ecore_evas_resize(ee, (int) edje_w, (int) edje_h);

    edje_object_signal_callback_add(edje, "*", "input", ui_input, "data");

    Ecore_Fd_Handler *handler = ecore_main_fd_handler_add(pipe_handler.input, ECORE_FD_READ, ui_event, NULL, NULL, NULL);
    ecore_main_fd_handler_active_set(handler, ECORE_FD_READ);

    g_debug("entering loop");
    ecore_main_loop_begin();
    g_debug("quit loop");
}

void ui_input(void *data, Evas_Object *obj, const char *emission, const char *source) {
    g_debug("ui_input()");
    if(phonegui_input_callback != NULL) {
        phonegui_input_callback(data, obj, emission, source);
    }
}

int ui_event(void *data, Ecore_Fd_Handler *fdh) {
    g_debug("ui_event()");
    int event = pipe_read(pipe_handler);

    if(phonegui_event_callback != NULL) {
        phonegui_event_callback(event); 
    }

    return 1;
}

