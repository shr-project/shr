#ifndef _PHONEGUI_CONTACTS_H
#define _PHONEGUI_CONTACTS_H

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>

void phonegui_contacts_show();
void phonegui_contacts_hide();

void contacts_input(void *data, Evas_Object *obj, const char *emission, const char *source);
void contacts_event(int event);

void retrieve_phonebook_callback(GError*error, GPtrArray*messages, gpointer userdata);
void get_phonebook_info_callback(GError *error, GHashTable *phonebook_info, gpointer userdata);
gint compare_entries(GValueArray **a, GValueArray **b);
void process_entry(GValueArray *entry);

void contacts_list_show();
void contacts_list_hide();
void contacts_options_show();
void contacts_loading_show();
void contacts_modify_show();
void contacts_modify_hide();
void contacts_new_show();
void contacts_new_hide();

#endif

