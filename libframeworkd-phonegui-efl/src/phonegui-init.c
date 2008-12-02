#include "phonegui-init.h"
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <Etk.h>
#include <Elementary.h>
#include <pthread.h>
#include <glib.h>
#include "async.h"

int event_callback(void *data, Ecore_Fd_Handler *fdh);

void ui_init() {

    // Initializations
    elm_init(phonegui_argc, phonegui_argv);
    g_debug("Initiated elementary");

    etk_init(phonegui_argc, phonegui_argv);
    g_debug("Initiated etk");

    // Add ecore exit callback
    /*if(phonegui_exit_cb != NULL) {
        g_debug("Added exit callback to ecore.");
        ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, phonegui_exit_cb, NULL);
    }*/

    // Add ecore pipe handler
    Ecore_Fd_Handler *handler = ecore_main_fd_handler_add(async_fd_input_get(), ECORE_FD_READ, event_callback, NULL, NULL, NULL);
    ecore_main_fd_handler_active_set(handler, ECORE_FD_READ);    

    elm_run();
}

void phonegui_init(int argc, char **argv, void (*exit_cb)()) {
    g_debug("phonegui_init()");

    // Assign arguments
    phonegui_argc = argc;
    phonegui_argv = argv;
    phonegui_exit_cb = exit_cb;

    // Initialize async manager
    async_init();

    // Create thread for ecore loop
    g_debug("Entering ecore loop");
    pthread_t thread;
    int rc = pthread_create(&thread, NULL, (void*) ui_init, NULL);
    if(rc) {
        g_error("Return code from pthread_create() is %d", rc);
    }
}



int event_callback(void *data, Ecore_Fd_Handler *fdh) {
    g_debug("event_callback()");
    async_dispatch();
    return 1;
}

