
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gthread.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib-bindings.h>
#include <frameworkd-glib/frameworkd-glib-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-call.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-network.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-device.h>
#include <frameworkd-glib/ousaged/frameworkd-glib-ousaged-dbus.h>
#include <frameworkd-glib/ousaged/frameworkd-glib-ousaged.h>
#include <frameworkd-glib/odeviced/frameworkd-glib-odeviced-idlenotifier.h>
#include <frameworkd-glib/odeviced/frameworkd-glib-odeviced-powersupply.h>
#include <frameworkd-glib/odeviced/frameworkd-glib-odeviced-audio.h>
#include <frameworkd-phonegui/frameworkd-phonegui.h>
#include "ophonekitd-frameworkd.h"
#include "ophonekitd-phonegui.h"
#include "ophonekitd-globals.h"

typedef struct {
	int id;
} call_t;

static gboolean sim_auth_active = FALSE;
static gboolean sim_ready = FALSE;
static gboolean gsm_ready = FALSE;
static gboolean gsm_available = FALSE;
static call_t *incoming_calls = NULL;
static call_t *outgoing_calls = NULL;
static int incoming_calls_size = 0;
static int outgoing_calls_size = 0;



void *
ophonekitd_frameworkd_main(void *_data)
{
	GMainContext *ctx = NULL;
	GMainLoop *loop = NULL;

	FrameworkdHandler *fwHandler;

	g_debug("[fso] starting frameworkd thread");

	ctx = g_main_context_new();
	//g_main_context_push_thread_default(ctx);
	loop = g_main_loop_new(ctx, FALSE);

	/* Register dbus handlers */
	g_debug("[fso] connect to frameworkd");
	fwHandler = frameworkd_handler_new();
	fwHandler->simAuthStatus = ophonekitd_sim_auth_status_handler;
	fwHandler->simReadyStatus = ophonekitd_sim_ready_status_handler;
	fwHandler->simIncomingStoredMessage =
		ophonekitd_sim_incoming_stored_message_handler;
	fwHandler->callCallStatus = ophonekitd_call_status_handler;
	fwHandler->deviceIdleNotifierState =
		ophonekitd_device_idle_notifier_state_handler;
	fwHandler->incomingUssd = ophonekitd_incoming_ussd_handler;
	fwHandler->usageResourceAvailable =
		ophonekitd_resource_available_handler;
	fwHandler->usageResourceChanged = ophonekitd_resource_changed_handler;


	DBusGConnection *bus = frameworkd_handler_connect(fwHandler);
	dbus_connection_setup_with_g_main(
			dbus_g_connection_get_connection(bus), loop);
	g_debug("[fso] connected to the buses");

	/* Start glib main loop and run list_resources() */
	g_debug("[fso] entering frameworkd glib main loop");
	g_timeout_add(0, list_resources, NULL);
	g_main_loop_run(loop);

	g_debug("[fso] finished mainloop");
	if (incoming_calls)
		free(incoming_calls);
	if (outgoing_calls)
		free(outgoing_calls);
}

void
ophonekitd_call_add(call_t ** calls, int *size, int id)
{
	g_debug("[fso] ophonekitd_call_add(%d)", id);
	(*size)++;
	if (*size == 1)
		*calls = malloc(sizeof(call_t));
	else
		*calls = realloc(*calls, sizeof(call_t) * (*size));
	(*calls)[(*size) - 1].id = id;
}

int
ophonekitd_call_check(call_t * calls, int *size, int id)
{
	int i;
	g_debug("[fso] ophonekitd_call_check(%d)", id);
	for (i = 0; i < (*size); i++) {
		if (calls[i].id == id)
			return i;
	}
	return -1;
}

void
ophonekitd_call_remove(call_t ** calls, int *size, int id)
{
	g_debug("[fso] ophonekitd_call_remove(%d)", id);
	if (*size == 1) {
		free(*calls);
		(*size)--;
		*calls = NULL;
	}
	else {
		int place = ophonekitd_call_check(*calls, size, id);
		if (place >= 0) {
			int i = place;
			for (i = place; i + 1 < (*size); i++) {
				(*calls)[i].id = (*calls)[i + 1].id;
			}
			(*size)--;
			*calls = realloc(*calls, sizeof(call_t) * (*size));
		}
	}
}



void
ophonekitd_device_idle_notifier_power_state_handler(GError * error,
						    const int status,
						    gpointer userdata)
{
	g_debug("[fso] power status: %d", status);
	if (incoming_calls_size == 0 && outgoing_calls_size == 0
	    && error == NULL && status != DEVICE_POWER_STATE_CHARGING
	    && status != DEVICE_POWER_STATE_FULL) {

		ousaged_suspend(NULL, NULL);
		g_debug("[fso] Suspend !");
	}
}

void
ophonekitd_device_idle_notifier_state_handler(const int state)
{
	g_debug("[fso] idle notifier state handler called, id %d", state);

	if (state == DEVICE_IDLE_STATE_SUSPEND) {
		odeviced_power_supply_get_power_status
			(ophonekitd_device_idle_notifier_power_state_handler,
			 NULL);
	}
}

void
ophonekitd_call_status_handler(const int call_id, const int status,
			       GHashTable * properties)
{
	g_debug("[fso] call status handler called, id: %d, status: %d", call_id,
		status);

	GValue *peerNumber = g_hash_table_lookup(properties, "peer");
	gchar *number;
	if (peerNumber != NULL) {
		int len;
		number = g_strdup_value_contents(peerNumber);
		len = strlen(number);
		/* FIXME: fix the ugly " bug
		 * we potentially waste a couple of bytes, fix it in a normal manner*/
		if (number[0] == '"') {
			gchar *tmp;
			tmp = strdup(number);
			strcpy(number, &tmp[1]);
			len--;
		}
		if (len > 0 && number[len - 1] == '"') {
			number[len - 1] = '\0';
		}

	}
	else {
		number = "*****";
	}

	switch (status) {
		case CALL_STATUS_INCOMING:
			g_debug("[fso] incoming call");
			if (ophonekitd_call_check
			    (incoming_calls, &incoming_calls_size,
			     call_id) == -1) {
				ophonekitd_call_add(&incoming_calls,
						    &incoming_calls_size,
						    call_id);
				ophonekitd_phonegui_call_command(
						INCOMING_CALL_SHOW, call_id,
						status, number);
			}
			break;
		case CALL_STATUS_OUTGOING:
			g_debug("[fso] outgoing call");
			if (ophonekitd_call_check
			    (outgoing_calls, &outgoing_calls_size,
			     call_id) == -1) {
				ophonekitd_call_add(&outgoing_calls,
						    &outgoing_calls_size,
						    call_id);
				ophonekitd_phonegui_call_command(
						OUTGOING_CALL_SHOW, call_id,
						status, number);
			}
			break;
		case CALL_STATUS_RELEASE:
			g_debug("[fso] release call");
			if (ophonekitd_call_check
			    (incoming_calls, &incoming_calls_size,
			     call_id) != -1) {
				ophonekitd_call_remove(&incoming_calls,
						       &incoming_calls_size,
						       call_id);
				ophonekitd_phonegui_command(INCOMING_CALL_HIDE,
						GINT_TO_POINTER(call_id));
			}
			if (ophonekitd_call_check
			    (outgoing_calls, &outgoing_calls_size,
			     call_id) != -1) {
				ophonekitd_call_remove(&outgoing_calls,
						       &outgoing_calls_size,
						       call_id);
				ophonekitd_phonegui_command(OUTGOING_CALL_HIDE,
						GINT_TO_POINTER(call_id));
			}
			break;
		case CALL_STATUS_HELD:
			g_debug("[fso] held call");
			break;
		case CALL_STATUS_ACTIVE:
			g_debug("[fso] active call");
			break;
		default:
			g_debug("[fso] Unknown CallStatus");
			break;
	}
}


void
ophonekitd_sim_auth_status_handler(const int status)
{
	g_debug("[fso] ophonekitd_sim_auth_status_handler()");
	if (status == SIM_READY) {
		g_debug("[fso] sim auth ready");

		if (sim_auth_active) {
			sim_auth_active = FALSE;
			ophonekitd_phonegui_command(SIM_AUTH_HIDE, 
					GINT_TO_POINTER(status));
		}
		power_up_antenna();
	}
	else {
		g_debug("[fso] sim not ready");
		if (!sim_auth_active) {
			sim_auth_active = TRUE;
			ophonekitd_phonegui_command(SIM_AUTH_SHOW,
					GINT_TO_POINTER(status));
		}
	}
}

void
sim_ready_actions(void)
{
	g_debug("[fso] sim ready");
	sim_ready = TRUE;
	ogsmd_sim_get_messagebook_info(get_messagebook_info_callback, NULL);
}



void
ophonekitd_sim_ready_status_handler(gboolean status)
{
	g_debug("[fso] ophonekitd_sim_ready_status_handler()");
	if (status)
		sim_ready_actions();
}


void
ophonekitd_sim_incoming_stored_message_handler(const int id)
{
	g_debug("[fso] ophonekitd_sim_incoming_stored_message_handler()");
	if (show_incoming_sms == TRUE) {
		ophonekitd_phonegui_command(MESSAGE_SHOW,
			GINT_TO_POINTER(id));
	}
	ogsmd_sim_get_messagebook_info(get_messagebook_info_callback, NULL);
}

void
ophonekitd_incoming_ussd_handler(int mode, const char *message)
{
	g_debug("[fso] ophonekitd_incoming_ussd_handler(mode=%d, message=%s)", mode,
		message);
	ophonekitd_phonegui_ussd_command(mode, message);
}




gboolean
list_resources()
{
	g_debug("[fso] list_resources()");
	if (!gsm_available)
		ousaged_list_resources(list_resources_callback, NULL);
	return FALSE;
}

void
list_resources_callback(GError * error, char **resources, gpointer userdata)
{
	g_debug("[fso] list_resources_callback()");
	if (error == NULL) {
		int i = 0;
		while (resources[i] != NULL) {
			g_debug("[fso] Resource %s available", resources[i]);
			if (!strcmp(resources[i], "GSM")) {
				gsm_available = TRUE;
				break;
			}
			i++;
		}

		if (gsm_available) {
			g_debug("[fso] Request GSM resource");
			ousaged_request_resource("GSM",
						 request_resource_callback,
						 NULL);
		}
	}
	else if (IS_FRAMEWORKD_GLIB_DBUS_ERROR
		 (error, FRAMEWORKD_GLIB_DBUS_ERROR_SERVICE_NOT_AVAILABLE)
		 || IS_FRAMEWORKD_GLIB_DBUS_ERROR(error,
						  FRAMEWORKD_GLIB_DBUS_ERROR_NO_REPLY)
		) {

		g_debug("[fso] dbus not available, try again in 5s");
		g_timeout_add(5000, list_resources, NULL);
	}
	else {
		g_debug("[fso] Unknown error, try again in 10s");
		g_timeout_add(10000, list_resources, NULL);
	}
}

gboolean
power_up_antenna()
{
	g_debug("[fso] call ogsmd_device_set_antenna_power()");
	ogsmd_device_set_antenna_power(TRUE, power_up_antenna_callback, NULL);
	return FALSE;
}

void
request_resource_callback(GError * error, gpointer userdata)
{
	g_debug("[fso] request_resource_callback()");

	if (error == NULL) {
		/* nothing to do when there is no error
		 * the signal handler for ResourceChanged
		 * will do the rest */
		return;
	}
	else if (IS_FRAMEWORKD_GLIB_DBUS_ERROR
		 (error, FRAMEWORKD_GLIB_DBUS_ERROR_SERVICE_NOT_AVAILABLE)
		 || IS_FRAMEWORKD_GLIB_DBUS_ERROR(error,
						  FRAMEWORKD_GLIB_DBUS_ERROR_NO_REPLY))
	{
		g_debug("[fso] dbus not available, try again in 5s");
		g_timeout_add(5000, list_resources, NULL);
	}
	else {
		/* FIXME: Remove this when frameworkd code is ready */
		g_debug("[fso] request resource error, try again in 5s");
		g_debug("[fso] error: %s %s %d", error->message,
			g_quark_to_string(error->domain), error->code);
		g_timeout_add(5000, list_resources, NULL);
	}
}


void
power_up_antenna_callback(GError * error, gpointer userdata)
{
	g_debug("[fso] power_up_antenna_callback()");
	if (error != NULL) {
		if (IS_SIM_ERROR(error, SIM_ERROR_AUTH_FAILED)) {
			/*
			 * This auth status query is needed for startup when
			 * there's no auth status signal emitted
			 */
			if (!sim_auth_active) {
				ogsmd_sim_get_auth_status
					(sim_auth_status_callback, NULL);
			}
			return;
		}
		else if (IS_SIM_ERROR(error, SIM_ERROR_NOT_PRESENT)) {
			g_message("SIM card not present.");
			ophonekitd_phonegui_command(DIALOG_SHOW,
				GINT_TO_POINTER(PHONEGUI_DIALOG_SIM_NOT_PRESENT));
			return;
		}
		else if (IS_RESOURCE_ERROR(error, RESOURCE_ERROR_NOT_ENABLED)) {
			g_message("GSM is not yet enabled, try again in 1s");
			g_timeout_add(1000, power_up_antenna, NULL);
		}
		else if (IS_FRAMEWORKD_GLIB_DBUS_ERROR
			 (error,
			  FRAMEWORKD_GLIB_DBUS_ERROR_SERVICE_NOT_AVAILABLE)
			 || IS_FRAMEWORKD_GLIB_DBUS_ERROR(error,
							  FRAMEWORKD_GLIB_DBUS_ERROR_NO_REPLY)
			) {
			g_debug("[fso] dbus not available, try again in 5s");
			g_timeout_add(5000, power_up_antenna, NULL);
			return;
		}
		else {
			g_debug("[fso] Unknown error: %s %s %d", error->message,
				g_quark_to_string(error->domain), error->code);
			g_timeout_add(5000, power_up_antenna, NULL);
			return;
		}
	}

	ogsmd_network_register(register_to_network_callback, NULL);
}

void
sim_auth_status_callback(GError * error, int status, gpointer userdata)
{
	g_debug("[fso] sim_auth_status_callback()");

	g_debug("[fso] sim_auth_active: %d", sim_auth_active);
	if (sim_auth_active) {
		sim_auth_active = FALSE;
		ophonekitd_phonegui_command(SIM_AUTH_HIDE,
			GINT_TO_POINTER(status));
	}

	if (status != SIM_READY) {
		if (!sim_auth_active) {
			sim_auth_active = TRUE;
			ophonekitd_phonegui_command(SIM_AUTH_SHOW,
				GINT_TO_POINTER(status));
		}
		return;
	}

	power_up_antenna();
}

void
sim_ready_status_callback(GError * error, gboolean status, gpointer userdata)
{
	/* if sim is already ready (by the ReadyStatus signal) - nothing to do */
	if (sim_ready)
		return;

	if (error) {
		g_debug("[fso] GetSimReady failed: %s %s %d", error->message,
			g_quark_to_string(error->domain), error->code);
		return;
	}

	g_debug("[fso] sim_ready_status_callback(status=%d)", status);
	if (status)
		sim_ready_actions();
}



void
register_to_network_callback(GError * error, gpointer userdata)
{
	g_debug("[fso] register_to_network_callback()");
	if (error == NULL) {
		/* Antenna works, registered to network */
	}
	else {
		g_debug("[fso] Registering to network failed: %s %s %d",
			error->message, g_quark_to_string(error->domain),
			error->code);
		/* FIXME */
	}
}

void
get_messagebook_info_callback(GError * error, GHashTable * info,
			      gpointer userdata)
{
	g_debug("[fso] get_messagebook_info_callback()");
	if (error == NULL && info != NULL) {
		gpointer p = NULL;
		int first = 0, last = 0, used = 0, total = 0;

		if ((p = g_hash_table_lookup(info, "first")) != NULL)
			first = g_value_get_int(p);
		else
			g_debug("[fso] get_messagebok_info_callback(): No value for \"first\"");

		if ((p = g_hash_table_lookup(info, "last")) != NULL)
			last = g_value_get_int(p);
		else
			g_debug("[fso] get_messagebok_info_callback(): No value for \"last\"");

		if ((p = g_hash_table_lookup(info, "used")) != NULL)
			used = g_value_get_int(p);
		else
			g_debug("[fso] get_messagebok_info_callback(): No value for \"used\"");

		total = last - first + 1;
		g_debug("[fso] messagebook info: first: %d, last %d, used: %d, total %d", first, last, used, total);
		if (used == total) {
			ophonekitd_phonegui_command(DIALOG_SHOW,
				GINT_TO_POINTER(PHONEGUI_DIALOG_MESSAGE_STORAGE_FULL));
		}
	}
	else {
		g_debug("[fso] MessageBookInfo failed: %s %s %d", error->message,
			g_quark_to_string(error->domain), error->code);
		/* TODO */
	}
	g_debug("[fso] get_messagebook_info_callback done");
}



void
ophonekitd_resource_available_handler(const char *name, gboolean availability)
{
	g_debug("[fso] resource %s is now %s", name,
		availability ? "available" : "vanished");
	if (strcmp(name, "GSM") == 0) {
		if (gsm_available ^ availability) {
			gsm_available = availability;
			if (gsm_available) {
				g_debug("[fso] Request GSM resource");
				ousaged_request_resource("GSM",
							 request_resource_callback,
							 NULL);
			}
		}
	}
}


void
ophonekitd_resource_changed_handler(const char *name, gboolean state,
				    GHashTable * attributes)
{
	gpointer p = NULL;
	g_debug("[fso] resource %s is now %s", name, state ? "enabled" : "disabled");
	p = g_hash_table_lookup(attributes, "policy");
	if (p)
		g_debug("[fso]    policy:   %d", g_value_get_int(p));
	p = g_hash_table_lookup(attributes, "refcount");
	if (p)
		g_debug("[fso]    refcount: %d", g_value_get_int(p));

	if (strcmp(name, "GSM") == 0) {
		/* check if state actually really changed for GSM */
		if (gsm_ready ^ state) {
			gsm_ready = state;
			if (gsm_ready) {
				/* TODO: remove this workaround when fsousaged
				 * sends the signal when the GSM resource
				 * actually is really ready - until then give
				 * it some time to get it ready ... */
				power_up_antenna();
				ogsmd_sim_get_sim_ready
					(sim_ready_status_callback, NULL);
			}
		}
	}
}

