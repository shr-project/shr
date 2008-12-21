/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              Marc-Olivier Barre <marco@marcochapeau.org>
 *              Julien Cassignol <ainulindale@gmail.com>
 *              Andreas Engelbredt Dalsgaard <andreas.dalsgaard@gmail.com>
 *              quickdev
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

#include "ophonekitd-main.h"
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <glib.h>
#include <glib/gthread.h>
#include <frameworkd-glib/frameworkd-glib-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-call.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-network.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-device.h>
#include <frameworkd-glib/ousaged/frameworkd-glib-ousaged.h>
#include <frameworkd-glib/odeviced/frameworkd-glib-odeviced-idlenotifier.h>
#include <frameworkd-glib/odeviced/frameworkd-glib-odeviced-powersupply.h>
#include <frameworkd-glib/odeviced/frameworkd-glib-odeviced-audio.h>
#include "ophonekitd-phonegui.h"
#include "ophonekitd-phonelog.h"
#include "ophonekitd-dbus.h"

typedef struct {
	int id;
	int unique_id;
} call_t;

gboolean sim_auth_active = FALSE;
call_t *incoming_calls = NULL;
call_t *outgoing_calls = NULL;
int incoming_calls_size = 0;
int outgoing_calls_size = 0;
GHashTable *contact_cache = NULL;

int main(int argc, char ** argv) {
    GMainLoop *mainloop = NULL;
    FrameworkdHandlers fwHandler;
    DBusGConnection *bus = NULL;
    DBusGProxy *bus_proxy = NULL;

    /* Load, connect and initiate phonegui library */
    phonegui_load("ophonekitd");
    phonegui_connect();
    phonegui_init(argc, argv, exit_callback);
    g_debug("Phonegui initiated");

    /* Initiate phonelog database */
    phonelog_init_database();
    g_debug("Phonelog database initiated");

    /* Register dbus handlers */
    fwHandler.networkStatus = NULL;
    fwHandler.networkSignalStrength = NULL;
    fwHandler.simAuthStatus = ophonekitd_sim_auth_status_handler;
    fwHandler.simIncomingStoredMessage = ophonekitd_sim_incoming_stored_message_handler;
    fwHandler.callCallStatus = ophonekitd_call_status_handler;
    fwHandler.deviceIdleNotifierState = ophonekitd_device_idle_notifier_state_handler;

    if (!g_thread_supported ())
        g_thread_init (NULL);

    dbus_g_thread_init ();

    
    mainloop = g_main_loop_new (NULL, FALSE);
    g_debug("Entering glib main loop");
    
    ophonekitd_dbus_start();
    
    dbus_connect_to_bus(&fwHandler);
    g_debug("Connected to the buses");
    
    /* Start glib main loop and run list_resources() */
    g_timeout_add(0, list_resources, NULL);
    g_main_loop_run(mainloop);

    /* Close phonelog database */
    phonelog_close_database();
    g_debug("Phonelog database closed");

    free(incoming_calls);
    free(outgoing_calls);
    g_hash_table_destroy(contact_cache);

    exit(EXIT_SUCCESS);
}


void ophonekitd_call_add(call_t **calls, int *size, int id, int unique_id) {
	g_debug("ophonekitd_call_add(%d, %u)", id, unique_id);
    (*size)++;
    if(*size == 1)
        *calls = malloc(sizeof(call_t));
    else
        *calls = realloc(calls, sizeof(call_t)*(*size));
    (*calls)[(*size)-1].id = id;
    (*calls)[(*size)-1].unique_id = unique_id;
}

int ophonekitd_call_check(call_t *calls, int *size, int id) {
    int i;
	g_debug("ophonekitd_call_check(%d)", id);
    for(i = 0; i < (*size) ; i++) {
        if(calls[i].id == id)
            return i;
    }
    return -1;
}

int ophonekitd_call_get_unique_id(call_t *calls, int *size, int id) {
	g_debug("ophonekitd_call_get_unique_id(%d)", id);
    int place = ophonekitd_call_check(calls, size, id);
    if(place >= 0) {
    	return calls[place].unique_id;
    }
    return -1;
}

void ophonekitd_call_remove(call_t *calls, int *size, int id) {
	g_debug("ophonekitd_call_remove(%d)", id);
    if(*size == 1)  {
        free(calls);
        (*size)--;
        calls = NULL;
    } else {
        int place = ophonekitd_call_check(calls, size, id);
        if(place >= 0) {
            int i = place;
            for(i = place; i + 1 < (*size); i++) {
                calls[i].id = calls[i+1].id;
                calls[i].unique_id = calls[i+1].unique_id;
            }
            (*size)--;
            calls = realloc(calls, sizeof(call_t)*(*size));
        }
    }
}



void ophonekitd_device_idle_notifier_power_state_handler(GError *error, const int status, gpointer userdata) {
    g_debug("power status: %d", status);
    if(
        incoming_calls_size == 0 && outgoing_calls_size == 0 && error == NULL &&
        status != DEVICE_POWER_STATE_CHARGING && status != DEVICE_POWER_STATE_FULL
    ) {
        ousaged_suspend(NULL, NULL);
        g_debug("Suspend !");
        /* Suspend is working on my kernel, but unfortunately resume isn't
         * I'll suggest to check if SHR kernel image can resume before
         * commenting out this line ! */
        /* As of 10/13/2008 :
         * Working linux-openmoko git revision :
         * f5b973489beb1a1239dfad53e3ad6e36ff7ee958*/
    }
}

void ophonekitd_device_idle_notifier_state_handler(const int state) {
    g_debug("idle notifier state handler called, id %d", state);

    if(state == DEVICE_IDLE_STATE_SUSPEND) {
        odeviced_power_supply_get_power_status(ophonekitd_device_idle_notifier_power_state_handler, NULL);
    }
}

void ophonekitd_call_status_handler(const int call_id, const int status, GHashTable *properties) {
    g_debug("call status handler called, id: %d, status: %d", call_id, status);

    GValue *peerNumber = g_hash_table_lookup(properties, "peer");
    gchar* number;
    if(peerNumber != NULL) {
        number = g_strdup_value_contents (peerNumber);
    }

    switch(status) {
        case CALL_STATUS_INCOMING:
            g_debug("incoming call");
            if(ophonekitd_call_check(incoming_calls, &incoming_calls_size, call_id) == -1) {
                int unique_id = phonelog_add_new_call(number);
                ophonekitd_call_add(&incoming_calls, &incoming_calls_size, call_id, unique_id);
                phonelog_log_call_event(unique_id, status);
                phonegui_incoming_call_show(call_id, status, cache_phonebook_lookup(number));
            }
            break;
        case CALL_STATUS_OUTGOING:
            g_debug("outgoing call");
            if(ophonekitd_call_check(outgoing_calls, &outgoing_calls_size, call_id) == -1) {
                int unique_id = phonelog_add_new_call(number);
                ophonekitd_call_add(&outgoing_calls, &outgoing_calls_size, call_id, unique_id);
                phonelog_log_call_event(unique_id, status);
                phonegui_outgoing_call_show(call_id, status, cache_phonebook_lookup(number));
            }
            break;
        case CALL_STATUS_RELEASE:
            g_debug("release call");
            if(ophonekitd_call_check(incoming_calls, &incoming_calls_size, call_id) != -1) {
                int unique_id =
                    ophonekitd_call_get_unique_id(incoming_calls, &incoming_calls_size, call_id);
                phonelog_log_call_event(unique_id, status);
                ophonekitd_call_remove(incoming_calls, &incoming_calls_size, call_id);
                phonegui_incoming_call_hide(call_id);
            }
            if(ophonekitd_call_check(outgoing_calls, &outgoing_calls_size, call_id) != -1) {
                int unique_id =
                    ophonekitd_call_get_unique_id(outgoing_calls, &outgoing_calls_size, call_id);
                phonelog_log_call_event(unique_id, status);
                ophonekitd_call_remove(outgoing_calls, &outgoing_calls_size, call_id);
                phonegui_outgoing_call_hide(call_id);
            }
            break;
        case CALL_STATUS_HELD:
            g_debug("held call");
            break;
        case CALL_STATUS_ACTIVE:
            g_debug("active call");
            if(ophonekitd_call_check(incoming_calls, &incoming_calls_size, call_id) != -1) {
                int unique_id =
                    ophonekitd_call_get_unique_id(incoming_calls, &incoming_calls_size, call_id);
                phonelog_log_call_event(unique_id, status);
            }
            if(ophonekitd_call_check(outgoing_calls, &outgoing_calls_size, call_id) != -1) {
                int unique_id =
                    ophonekitd_call_get_unique_id(outgoing_calls, &outgoing_calls_size, call_id);
                phonelog_log_call_event(unique_id, status);
            }
            break;
        default:
            g_error("Unknown CallStatus");
    }
}


void ophonekitd_sim_auth_status_handler(const int status) {
    g_debug("ophonekitd_sim_auth_status_handler()");
    if(status == SIM_READY) {
        g_debug("sim ready");

        if(sim_auth_active) {
            sim_auth_active = FALSE;
            phonegui_sim_auth_hide(status);
        }
        ogsmd_network_register(register_to_network_callback, NULL);
        ogsmd_sim_get_messagebook_info(get_messagebook_info_callback, NULL);
        ogsmd_sim_retrieve_phonebook("contacts", cache_phonebook_callback, NULL);
    } else {
        g_debug("sim not ready");
        if(!sim_auth_active) {
            sim_auth_active = TRUE;
            phonegui_sim_auth_show(status);
        }
    }
}


void ophonekitd_sim_incoming_stored_message_handler(const int id) {
    g_debug("ophonekitd_sim_incoming_stored_message_handler()");

    /* FIXME: Quick and dirty hack to avoid the GSM not enable error after suspend */
    sleep(10);

    phonegui_message_show(id);
    ogsmd_sim_get_messagebook_info(get_messagebook_info_callback, NULL);
}


gboolean list_resources() {
    g_debug("list_resources()");
    ousaged_list_resources(list_resources_callback, NULL);
    return FALSE;
}

void list_resources_callback(GError *error, char** resources, gpointer userdata) {
    g_debug("list_resources_callback()");
    if(error == NULL) {
        gboolean available = FALSE;
        int i = 0;
        while(resources[i] != NULL) {
            g_debug("Resource %s available", resources[i]);
            if(!strcmp(resources[i], "GSM")) {
                available = TRUE;
                break;
            }
            i++;
        }

        if(available) {
            g_debug("Request GSM resource");
            ousaged_request_resource("GSM", request_resource_callback, NULL);
        } else {
            g_debug("GSM not available, try again in 5s");
            g_timeout_add(5000, list_resources, NULL);
        }
    } else if(IS_DBUS_ERROR(error, DBUS_ERROR_SERVICE_NOT_AVAILABLE) || IS_DBUS_ERROR(error, DBUS_ERROR_NO_REPLY)) {
        g_debug("dbus not available, try again in 5s");
        g_timeout_add(5000, list_resources, NULL);
    } else {
        g_error("Unknown error");
    }
}

gboolean power_up_antenna() {
        g_debug("call ogsmd_device_set_antenna_power()");
        ogsmd_device_set_antenna_power(TRUE, power_up_antenna_callback, NULL);
	return FALSE;
}

void request_resource_callback(GError *error, gpointer userdata) {
    g_debug("request_resource_callback()");

    if(error == NULL) {
	power_up_antenna();
    } else if(IS_DBUS_ERROR(error, DBUS_ERROR_SERVICE_NOT_AVAILABLE) || IS_DBUS_ERROR(error, DBUS_ERROR_NO_REPLY)) {
        g_debug("dbus not available, try again in 5s");
        g_timeout_add(5000, list_resources, NULL);
    } else {
        /* FIXME: Remove this when frameworkd code is ready */
        g_debug("request resource error, try again in 5s");
        g_error("error: %s %s %d", error->message, g_quark_to_string(error->domain), error->code);
        g_timeout_add(5000, list_resources, NULL);
    }
}


void power_up_antenna_callback(GError *error, gpointer userdata) {
    g_debug("power_up_antenna_callback()");
    if(error != NULL) {
        if(IS_SIM_ERROR(error, SIM_ERROR_AUTH_FAILED)) {

            /*
             * This auth status query is needed for startup when
             * there's no auth status signal emitted
             */
            if(!sim_auth_active) {
                ogsmd_sim_get_auth_status(sim_auth_status_callback, NULL);
            }

        } else if(IS_SIM_ERROR(error, SIM_ERROR_NOT_PRESENT)) {
            g_error("SIM card not present.");
        } else if(IS_DBUS_ERROR(error, DBUS_ERROR_SERVICE_NOT_AVAILABLE) || IS_DBUS_ERROR(error, DBUS_ERROR_NO_REPLY)) {
            g_debug("dbus not available, try again in 5s");
            g_timeout_add(5000, power_up_antenna, NULL);
        } else {
            g_error("Unknown error: %s %s %d", error->message, g_quark_to_string(error->domain), error->code);
        }
    }
}

void sim_auth_status_callback(GError *error, int status, gpointer userdata) {
    g_debug("sim_auth_status_callback()");

    g_debug("sim_auth_active: %d", sim_auth_active);
    if(sim_auth_active) {
        sim_auth_active = FALSE;
        phonegui_sim_auth_hide(status);
    }

    if(status == SIM_READY) {
        ogsmd_network_register(register_to_network_callback, NULL);
    } else {
        if(!sim_auth_active) {
            sim_auth_active = TRUE;
            phonegui_sim_auth_show(status);
        }
    }
}

void register_to_network_callback(GError *error, gpointer userdata) {
    g_debug("register_to_network_callback()");
    if(error == NULL) {
        /* Antenna works, registered to network */
    } else {
        /* TODO */
    }
}

void get_messagebook_info_callback(GError *error, GHashTable *info, gpointer userdata) {
    g_debug("get_messagebook_info_callback()");
    if(error == NULL && info != NULL) {
        gpointer p = NULL;
        int first = 0, last = 0, used = 0, total = 0;
        if((p = g_hash_table_lookup(info, "first")) != NULL)
                first = g_value_get_int(p);
        else
                g_debug("get_messagebok_info_callback(): No value for \"first\"");
        if((p = g_hash_table_lookup(info, "last")) != NULL)
                last = g_value_get_int(p);
        else
                g_debug("get_messagebok_info_callback(): No value for \"last\"");
        if((p = g_hash_table_lookup(info, "used")) != NULL)
                used = g_value_get_int(p);
        else
                g_debug("get_messagebok_info_callback(): No value for \"used\"");

        total = last - first + 1;
        g_debug("messagebook info: first: %d, last %d, used: %d, total %d", first, last, used, total);
        if(used == total) {
            phonegui_dialog_show(PHONEGUI_DIALOG_MESSAGE_STORAGE_FULL);
        }
    } else {
        /* TODO */
    }
}


void cache_phonebook_callback(GError *error, GPtrArray *contacts, gpointer userdata) {
    g_debug("creating contact_cache");
    contact_cache = g_hash_table_new(g_str_hash, g_str_equal);
    if (!contact_cache) {
        g_error("could not allocate contact cache");
        return;
    }
    g_ptr_array_foreach(contacts, cache_phonebook_entry, NULL);
}


void cache_phonebook_entry(GValueArray *entry, void *data) {
    char *number = strdup(g_value_get_string(g_value_array_get_nth(entry, 2)));
    char *name = strdup(g_value_get_string(g_value_array_get_nth(entry, 1)));
    g_hash_table_insert(contact_cache, number, name);
}


char *cache_phonebook_lookup(char *number) {
    if (!number || !*number)
        return ("unknown");
    if (*number == '"') {
        number++;
        char *s = number;
        while (*s) {
            if (*s == '"') {
                *s = '\0';
                break;
            }
            s++;
        }
    }

    char *name = g_hash_table_lookup(contact_cache, number);
    g_debug("looking for '%s' in contacts_cache", number);
    if (name)
        g_debug("found name '%s'", name);
    if (name && *name)
        return (name);
    return (number);
}


int exit_callback(void *data, int type, void *event) {
    /* called on ctrl-c, kill $pid, SIGINT, SIGTERM and SIGQIT */
    g_debug("exit_callback()");

    return 0;
}

