#ifndef _PHONEGUI_SIM_AUTH_H
#define _PHONEGUI_SIM_AUTH_H

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>

void phonegui_sim_auth_show(const int status);
void phonegui_sim_auth_hide(const int status);

void sim_auth_event(int event);
void sim_auth_update();
void sim_auth_clear();

void pin_callback(GError* error, gpointer data);
void puk_callback(GError* error, gpointer data);
int pins_different_callback(void *data);
void sim_auth_callback(GError *error, int status, gpointer userdata);

/* vibrator hack */
void vibrator_write(char* str);
void vibrator_enable();
void vibrator_disable();

void sim_auth_keypad_clicked(void *data, Evas_Object *obj, char input);
void sim_auth_delete_clicked();
void sim_auth_ok_clicked();

void sim_auth_input_show();
void sim_auth_input_hide();
void sim_auth_checking_show();
void sim_auth_pins_different_show();
void sim_auth_pin_correct_show();
void sim_auth_pin_wrong_show();
void sim_auth_puk_correct_show();
void sim_auth_puk_wrong_show();

#endif
