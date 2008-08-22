/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              Marc-Olivier Barre <marco@marcochapeau.org>
 *              Julien Cassignol <ainulindale@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Public License as published by
 *  the Free Software Foundation; version 2 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <libjana/jana.h>
#include <libjana/jana-store.h>
#include <libjana-ecal/jana-ecal.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <frameworkd-glib/frameworkd-glib-dbus.h>
#include <frameworkd-glib/frameworkd-glib-call.h>
#include <frameworkd-glib/frameworkd-glib-sim.h>
#include <frameworkd-glib/frameworkd-glib-network.h>
#include <frameworkd-glib/frameworkd-glib-device.h>
#include "ophonekitd-main.h"
#include <frameworkd-phonegui/frameworkd-phonegui.h>

static gboolean connected_to_network = FALSE;
static JanaStore* sms_store;

/*static MokoJournal *journal = NULL;
static MokoContacts *contacts = NULL;

typedef struct {
    int id;
    int status;
    MokoJournalEntry *journal_entry;
    MokoTime *time;
    MokoContactsEntry *contacts_entry;
} OphonekitdCallStatus;
*/

void ophonekitd_sim_retrieve_messagebook_callback(GError* error, GPtrArray* messages, gpointer userdata);
void ophonekitd_sim_delete_message_callback(GError* error, gpointer userdata);
void ophonekitd_sms_store_opened_cb (JanaStore *store);

int main(int argc, char ** argv) {
        GMainLoop *mainloop = NULL;
        FrameworkdHandlers fwHandler;

        g_type_init();

#ifdef DEBUG
        printf("Starting...\n");
#endif
        mainloop = g_main_loop_new (NULL, FALSE);

        fwHandler.networkStatus = ophonekitd_network_status_handler;
        fwHandler.simAuthStatus = ophonekitd_sim_auth_status_handler;
        fwHandler.simIncomingMessage = ophonekitd_sim_incoming_message_handler;
        fwHandler.callCallStatus = ophonekitd_call_status_handler;
        fwHandler.smsMessageSent = ophonekitd_sms_message_sent_handler;
        fwHandler.smsIncomingMessage = ophonekitd_sms_incoming_message_handler;
        fwHandler.networkSignalStrength = NULL;

        dbus_connect_to_bus(&fwHandler);
        phonegui_init(argc, argv);
#ifdef DEBUG
        printf("Connected to the buses\n");
#endif
        g_timeout_add(5000, power_up_antenna, NULL);
        sms_store = jana_ecal_store_new (JANA_COMPONENT_NOTE);
        g_signal_connect (sms_store, "opened",
                    G_CALLBACK (ophonekitd_sms_store_opened_cb), NULL);
		jana_store_open (sms_store);
		g_debug("Opening memoes/notes");

#ifdef DEBUG
        printf("Entering mainloop.\n");
#endif
        g_main_loop_run (mainloop);

        exit(EXIT_SUCCESS);

}

void ophonekitd_sms_store_opened_cb (JanaStore *store)
{
#ifdef DEBUG
   printf("JanaStore is now opened\n");
#endif
}

gboolean power_up_antenna() {
        device_set_antenna_power(TRUE, power_up_antenna_callback, NULL);

        return connected_to_network; // End timeout
}

void power_up_antenna_callback(GError *error, gpointer userdata) {
        if(error != NULL) {
            if(IS_SIM_ERROR(error, SIM_ERROR_AUTH_FAILED)) {
                ophonekitd_display_code_UI();
            }
                /* TODO */
        }
        else {
                network_register(register_to_network_callback, NULL);
        }
}

void register_to_network_callback(GError *error, gpointer userdata) {
        if(error != NULL) {
                /* TODO */
        } else {
                /* Antenna works, registered to network */
                connected_to_network = TRUE;
        }
}

static void print_property(gpointer key, gpointer value, gpointer user_data)
{
	printf( "Call Property :: %s = %s\n", (char*) key, (char*) value );
}

void ophonekitd_call_status_handler(const int id_call, const int status, GHashTable *properties) {
	gchar *number = NULL;

    switch(status) {
        case CALL_STATUS_INCOMING:
        case CALL_STATUS_OUTGOING:
           break; 
    }

	/* NOTE:
	 *
	 * This is just test code.
	 */

	if (properties)
		g_hash_table_foreach(properties, print_property, NULL);

	g_warning("Call status: %d", id_call);
	phonegui_display_call_UI(id_call, status, number);
}

void ophonekitd_network_status_handler(GHashTable *status) {
}

void ophonekitd_sim_auth_status_handler(const int status) {
        if(status == SIM_READY) {
                phonegui_destroy_pin_UI();
        }
        else {
                phonegui_display_pin_UI(status);
        }
#ifdef DEBUG
        printf ("Auth status handler calling the UI on a %i signal", status);
#endif

}

void ophonekitd_sms_message_sent_handler(const int id, gboolean success, const char* reason) {
}

void ophonekitd_sms_incoming_message_handler(const int id) {
}

void ophonekitd_sim_incoming_message_handler(const int id) {
  sim_retrieve_messagebook("unread", ophonekitd_sim_retrieve_messagebook_callback, NULL);
}

static void add_meassage_to_pim(gpointer data, gpointer user_data) {

  GValueArray* message = data; 
  const int index = g_value_get_int(g_value_array_get_nth(message, 0));
  const char* status = g_value_get_string(g_value_array_get_nth(message, 1));
  const char* number = g_value_get_string(g_value_array_get_nth(message, 2));
  const char* content = g_value_get_string(g_value_array_get_nth(message, 3));
  
  JanaNote *note = jana_ecal_note_new ();
  #ifdef DEBUG
    g_debug ("Add message to pim: \ncontent: %s\nstatus: %s\n", content, status);
  #endif

  jana_note_set_author (note, number);
  jana_note_set_body (note, content);
    
  /* TODO: Set timestamps */
  jana_component_set_categories (JANA_COMPONENT (note), (const gchar *[]){ status, NULL});

  /* Add SMS to store */
  jana_store_add_component (sms_store, JANA_COMPONENT (note));

  sim_delete_message(index, ophonekitd_sim_delete_message_callback, NULL);
}

void ophonekitd_sim_delete_message_callback(GError* error, gpointer userdata) {
  if (error) {
    g_debug("Error deleting message: %s", error->message);
  }
}

void ophonekitd_sim_retrieve_messagebook_callback(GError* error, GPtrArray* messages, gpointer userdata) {
   
   if (!error) {
#ifdef DEBUG   
     g_debug("No error deleting message");
#endif
   } else {
     g_ptr_array_foreach(messages, add_meassage_to_pim, NULL);
   }
}

void ophonekitd_display_code_UI_callback(GError* error, int status, gpointer userdata) {
    if(error != NULL) {
        /* TODO */
    } else if (status != SIM_READY) {
        phonegui_display_pin_UI(status);
    }
}

void ophonekitd_display_code_UI () {
    sim_get_auth_status (ophonekitd_display_code_UI_callback, NULL);
}

