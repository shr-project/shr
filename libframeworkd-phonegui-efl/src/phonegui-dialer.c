#include "phonegui-dialer.h"
#include <pthread.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-call.h>
#include "pipe.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/dialer.edj"

static PipeHandler pipe_handler;

static Ecore_Evas   *ee;
static Evas         *evas;
static Evas_Object  *edje;
static double       edje_w, edje_h;

static char number[64];
static int  number_length = 0;


void phonegui_dialer_launch(int argc, char** argv) {
    g_debug("phonegui_dialer_launch()");

    /* Create pipe */
    pipe_handler = pipe_create();
    g_debug("pipe created");

    ecore_init();
    ecore_evas_init();
    ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
    ecore_evas_title_set(ee, "openmoko-dialer3");
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

    edje_object_signal_callback_add(edje, "*", "input", dialer_ui_input, "data");

    Ecore_Fd_Handler *handler = ecore_main_fd_handler_add(pipe_handler.input, ECORE_FD_READ, dialer_ui_event, NULL, NULL, NULL);
    ecore_main_fd_handler_active_set(handler, ECORE_FD_READ);

    ecore_main_loop_begin();
}

void dialer_ui_input(void *data, Evas_Object *obj, const char *emission, const char *source) {
    g_debug("dialer_ui_input() input: %s", emission);

    if(!strcmp(emission, "call"))
    {
        g_debug("initiate call");
        ogsmd_call_initiate(number, "voice", dialer_call_initiate_callback, NULL);
    }
    else if(!strcmp(emission, "options"))
    {
    }
    else if(!strcmp(emission, "delete"))
    {
        if(number_length > 0)
        {
            number_length--;
            number[number_length] = '\0';
            dialer_ui_update();
        }
    }
    else
    {
        if(number_length < 64)
        {
            strncat(number, emission, 1);
            number_length++;
            dialer_ui_update();
        }
    }
}

void dialer_ui_event(void *data, Ecore_Fd_Handler *fdh) {
    g_debug("dialer_ui_event()");
}

void dialer_ui_update() {
    g_debug("dialer_ui_update()");
    edje_object_part_text_set(edje, "number", number);
}

void dialer_call_initiate_callback(GError *error, int call_id, gpointer userdata) {
    g_debug("call_initiate_callback()");

    /* Exit */
    ecore_main_loop_quit();
}


