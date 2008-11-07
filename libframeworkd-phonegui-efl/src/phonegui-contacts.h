#ifndef _PHONEGUI_CONTACTS_H
#define _PHONEGUI_CONTACTS_H

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>

void phonegui_contacts_show();
void phonegui_contacts_hide();

void contacts_event(int event);
void contacts_delete(void *data, Evas_Object *obj, void *event_info);

void retrieve_phonebook_callback(GError*error, GPtrArray*messages, gpointer userdata);
void get_phonebook_info_callback(GError *error, GHashTable *phonebook_info, gpointer userdata);
gint compare_entries(GValueArray **a, GValueArray **b);
void process_entry(GValueArray *entry, gpointer userdata);

void contacts_button_new_clicked();
void contacts_button_call_clicked();
void contacts_button_options_clicked();
void contacts_button_modify_clicked();
void contacts_button_back_clicked();
void contacts_button_delete_clicked();
void contacts_button_save_clicked();
void contacts_button_delete_yes_clicked();
void contacts_button_delete_no_clicked();

void contacts_list_show();
void contacts_list_hide();
void contacts_options_show();
void contacts_options_hide();
void contacts_loading_show();
void contacts_modify_show();
void contacts_modify_hide();
void contacts_new_show();
void contacts_new_hide();
void contacts_delete_show();
void contacts_delete_hide();

#endif

