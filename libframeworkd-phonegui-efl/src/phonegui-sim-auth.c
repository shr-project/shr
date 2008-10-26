#include "phonegui-sim-auth.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> /* for vibrator hack */
#include <unistd.h> /* for sleep */
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>
#include "pipe.h"
#include "phonegui-init.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/sim-auth.edj"

enum SimAuthModes {
    MODE_PIN,
    MODE_PUK,
    MODE_PUK_NEW_PIN,
    MODE_PUK_NEW_PIN_CONFIRM
};

enum SimAuthEvents {
    EVENT_CREATE,
    EVENT_MODE_PIN,
    EVENT_MODE_PUK,
    EVENT_PIN_CORRECT,
    EVENT_PIN_WRONG,
    EVENT_PUK_CORRECT,
    EVENT_PUK_WRONG,
    EVENT_HIDE
};


static int mode;
static char stars[13];

/* PIN */
static char pin[9];
static int  pin_length = 0;

/* PUK */
static char puk[13];
static int  puk_length = 0;
static char pin_confirm[9];
static int  pin_confirm_length = 0;


void phonegui_sim_auth_show(const int status) {
    g_debug("phonegui_sim_auth_show()");
    pipe_write(pipe_handler, sim_auth_event, EVENT_CREATE);

    if(status == SIM_PIN_REQUIRED) {
        mode = MODE_PIN;
        pipe_write(pipe_handler, sim_auth_event, EVENT_MODE_PIN);
    } else if(status == SIM_PUK_REQUIRED) {
        mode = MODE_PUK;
        pipe_write(pipe_handler, sim_auth_event, EVENT_MODE_PUK);
    } else {
        g_error("Unknown sim auth status: %d", status);
    }
}

void phonegui_sim_auth_hide(const int status) {
    g_debug("phonegui_sim_auth_hide");

    if(mode == MODE_PIN) {
        pipe_write(pipe_handler, sim_auth_event, EVENT_PIN_CORRECT);
    } else {
        pipe_write(pipe_handler, sim_auth_event, EVENT_PUK_CORRECT);
    }

    sleep(1);    
    pipe_write(pipe_handler, sim_auth_event, EVENT_HIDE);
}


void sim_auth_delete(Ecore_Evas *ee) {
    g_debug("sim_auth_delete");
    pipe_write(pipe_handler, sim_auth_event, EVENT_HIDE);
}


void sim_auth_event(int event) {
    g_debug("sim_auth_event()");
    g_debug("signal: %d", event);

    if(event == EVENT_CREATE) {
        g_debug("create");
        window_create("SIM Authentication", sim_auth_input, sim_auth_event, sim_auth_delete);
    } else if(event == EVENT_MODE_PIN) {
        g_debug("pin mode");
        edje_object_file_set(edje, UI_FILE, "sim_auth_input");
        edje_object_part_text_set(edje, "instruction", "Enter PIN:"); 
        ecore_evas_show(ee);
    } else if(event == EVENT_MODE_PUK) {
        g_debug("puk mode");
        edje_object_file_set(edje, UI_FILE, "sim_auth_input");
        edje_object_part_text_set(edje, "instruction", "Enter PUK:"); 
        ecore_evas_show(ee);
    } else if(event == EVENT_PIN_CORRECT) {
        edje_object_file_set(edje, UI_FILE, "sim_auth_pin_correct");        
    } else if(event == EVENT_PIN_WRONG) {
        edje_object_file_set(edje, UI_FILE, "sim_auth_pin_wrong");
        sim_auth_clear();
    } else if(event == EVENT_PUK_CORRECT) {
        edje_object_file_set(edje, UI_FILE, "sim_auth_puk_correct");
    } else if(event == EVENT_PUK_WRONG) {
        edje_object_file_set(edje, UI_FILE, "sim_auth_puk_wrong");
        sim_auth_clear();
    } else if(event == EVENT_HIDE) {
        g_debug("hide it!");
        sim_auth_clear();
        ecore_evas_hide(ee);
        edje_object_file_set(edje, UI_FILE, "sim_auth_input");
    } else {
        g_error("Unknown event %d", event);
    }
}


void sim_auth_input(void *data, Evas_Object *o, const char *emission, const char *source)
{
    g_debug("sim_auth_input()");

    char* string;
    int* length;
    if(mode == MODE_PIN || mode == MODE_PUK_NEW_PIN) {
        string = pin;
        length = &pin_length;
    } else if(mode == MODE_PUK) {
        string = puk;
        length = &puk_length;
    } else {
        string = pin_confirm;
        length = &pin_confirm_length;
    }
    g_debug("String: %s", string);
    g_debug("Length: %d", *length);
    g_debug("INPUT: %s", emission);

	if(!strcmp(emission, "ok"))
    {
        if(mode == MODE_PIN && strcmp(pin, "") != 0) {
            g_debug("Send pin");
            edje_object_file_set(edje, UI_FILE, "sim_auth_checking");
            ogsmd_sim_send_auth_code(pin, pin_callback, NULL);
        } else if(mode == MODE_PUK && strcmp(puk, "") != 0) {
            g_debug("Ask for a new PIN");
            edje_object_part_text_set(edje, "instruction", "Enter new PIN:"); 
            mode = MODE_PUK_NEW_PIN;
            sim_auth_update();
        } else if(mode == MODE_PUK_NEW_PIN && strcmp(pin, "") != 0) {
            g_debug("Ask for new PIN confirmation");
            edje_object_part_text_set(edje, "instruction", "Confirm new PIN:"); 
            mode = MODE_PUK_NEW_PIN_CONFIRM;
            sim_auth_update();
        } else if(mode == MODE_PUK_NEW_PIN_CONFIRM) {
            g_debug("See if PINs are identical");
            if(!strcmp(pin, pin_confirm)) {
                g_debug("PINs identical, send PUK and new PIN");
                edje_object_file_set(edje, UI_FILE, "sim_auth_checking");
                ogsmd_sim_unlock(puk, pin, puk_callback, NULL);
            } else {
                g_debug("PINs different");
                edje_object_file_set(edje, UI_FILE, "sim_auth_pins_different");
                vibrator_enable();
                ecore_timer_add(2, pins_different_callback, NULL);
            }
        }
    } else if(!strcmp(emission, "delete") && (*length) > 0) {
        (*length)--;
        string[*length] = 0;
        sim_auth_update();
    } else {
        /* 1,2,3 etc. */
        if(*length < 8)
        {
            strncat(string, emission, 1);
            (*length)++;
            sim_auth_update();
        }
    }
}


void sim_auth_update() {
    g_debug("sim_auth_update()");

    int length;
    if(mode == MODE_PUK)
        length = puk_length;
    else if(mode == MODE_PIN || mode == MODE_PUK_NEW_PIN)
        length = pin_length;
    else // if(mode == MODE_PUK_NEW_PIN_CONFIRM)
        length = pin_confirm_length;

	stars[0] = 0;
	int i;
	for(i = 0 ; i < length ; i++)
        strncat(stars, "*", 1);
	edje_object_part_text_set(edje, "input_text", stars); 
}

void sim_auth_clear() {
    pin[0] = 0;
    pin_length = 0;
    puk[0] = 0;
    puk_length = 0;
    pin_confirm[0] = 0;
    pin_confirm_length = 0;
    sim_auth_update();
}


void pin_callback(GError* error, gpointer data) {
    g_debug("pin_callback()");
    if(error != NULL) {
        g_debug("error");
        if(IS_SIM_ERROR(error, SIM_ERROR_AUTH_FAILED)) {
            pipe_write(pipe_handler, sim_auth_event, EVENT_PIN_WRONG);
            vibrator_enable();
            sleep(2);
            vibrator_disable();

            ogsmd_sim_get_auth_status(sim_auth_callback, NULL);
        } else {
            g_error("Unknown error");
        }
    }
}

void puk_callback(GError* error, gpointer data) {
    g_debug("puk_callback()");
    if(error != NULL) {
        g_debug("error code: %d %s %s", error->code, error->message, g_quark_to_string(error->domain));
        if(IS_SIM_ERROR(error, SIM_ERROR_AUTH_FAILED)) {
            mode = MODE_PUK;
            pipe_write(pipe_handler, sim_auth_event, EVENT_PUK_WRONG);
            vibrator_enable();
            sleep(2);
            vibrator_disable();
            pipe_write(pipe_handler, sim_auth_event, EVENT_MODE_PUK);
        }
    }
}


int pins_different_callback(void *data) {
    vibrator_disable();

    edje_object_file_set(edje, UI_FILE, "sim_auth_input");
    edje_object_part_text_set(edje, "instruction", "Enter PUK:"); 
    sim_auth_clear();
    mode = MODE_PUK;

    return 0;
}

void sim_auth_callback(GError *error, int status, gpointer userdata) {
    g_debug("sim_auth_callback()");
    if(status == SIM_PIN_REQUIRED) {
        mode = MODE_PIN;
        pipe_write(pipe_handler, sim_auth_event, EVENT_MODE_PIN);
    } else if(status == SIM_PUK_REQUIRED) {
        mode = MODE_PUK;
        pipe_write(pipe_handler, sim_auth_event, EVENT_MODE_PUK);
    } else {
        g_error("Unhandled sim auth status: %d", status);
    }
}




/*
 * Vibrator hack
 */
void vibrator_write(char* str) {
    g_debug("vibrator_write()");

    FILE *f = fopen("/sys/bus/platform/drivers/neo1973-vibrator/neo1973-vibrator.0/leds/neo1973\\:vibrator/brightness", "w");
    if(f == NULL) {
        g_error("fopen() failed");
    }
    int r = fputs(str, f);
    if(r < 0) {
        g_error("fputs() failed");
    }
    fclose(f);

}

void vibrator_enable() {
    g_debug("vibrator_enable()");
    //vibrator_write("255\n");
}

void vibrator_disable() {
    g_debug("vibrator_disable()");
    //vibrator_write("0\n");
}

