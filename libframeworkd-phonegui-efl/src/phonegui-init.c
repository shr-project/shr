#include "phonegui-init.h"
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <pthread.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>
#include "phonegui-sim-auth.h"
#include "phonegui-incoming-call.h"
#include "phonegui-outgoing-call.h"


void ui_input(void *data, Evas_Object *obj, const char *emission, const char *source) {
    g_debug("ui_input()");
    if(phonegui_mode == MODE_SIM_AUTH) {
        sim_auth_ui_input(data, obj, emission, source);
    } else if(phonegui_mode == MODE_INCOMING_CALL) {
        incoming_call_ui_input(data, obj, emission, source);
    } else if(phonegui_mode == MODE_OUTGOING_CALL) {
        outgoing_call_ui_input(data, obj, emission, source);
    } else {
        g_error("Unknown mode");
    }
}

void ui_event(void *data, Ecore_Fd_Handler *fdh) {
    g_debug("ui_event()");
    if(phonegui_mode == MODE_SIM_AUTH) {
        sim_auth_ui_event(data, fdh);
    } else if(phonegui_mode == MODE_INCOMING_CALL) {
        incoming_call_ui_event(data, fdh);
    } else if(phonegui_mode == MODE_OUTGOING_CALL) {
        outgoing_call_ui_event(data, fdh);
    } else {
        g_error("Unknown mode");
    }
}

void ui_init() {
    g_debug("ui_init()");

    pipe_handler = pipe_create();
    g_debug("pipe created");

    ecore_init();
    ecore_evas_init();
    ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
    ecore_evas_title_set(ee, "ophonekitd");
    ecore_evas_borderless_set(ee, 0);
    ecore_evas_shaped_set(ee, 0);

    evas = ecore_evas_get(ee);
    evas_font_path_append(evas, "/usr/local/share/edje/data/test/fonts/");

    edje_init();
    edje = edje_object_add(evas);
    //edje_object_file_set(edje, UI_FILE, "sim_auth_input");
    evas_object_move(edje, 0, 0);
    //edje_object_size_min_get(edje, &edje_w, &edje_h);
    evas_object_resize(edje, 480, 600);
    evas_object_show(edje);
    ecore_evas_resize(ee, (int)edje_w, (int)edje_h);

    edje_object_signal_callback_add(edje, "*", "input", ui_input, "data");

    Ecore_Fd_Handler *handler = ecore_main_fd_handler_add(pipe_handler.input, ECORE_FD_READ, ui_event, NULL, NULL, NULL);
    ecore_main_fd_handler_active_set(handler, ECORE_FD_READ);

    g_debug("entering loop");
    ecore_main_loop_begin();
    g_debug("quit loop");
}


void phonegui_init(int argc, char** argv) {

    g_debug("phonegui_init()");

    /* This two threads will run all the time. FIXME? */
    pthread_t thread;
    int rc;

    g_debug("starting sim_auth_ui_init()");
    rc = pthread_create(&thread, NULL, (void*) ui_init, NULL);
    if(rc) {
        g_error("Return code from pthread_create() is %d\n", rc);
    }
}

