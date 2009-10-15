
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
#include "ophonekitd-phoneuid-dbus.h"
#include "ophonekitd-fso.h"
#include "ophonekitd-globals.h"

typedef struct {
	int id;
} call_t;

static gboolean sim_auth_active = FALSE;
static gboolean sim_ready = FALSE;
static gboolean gsm_available = FALSE;
static gboolean gsm_ready = FALSE;
static call_t *incoming_calls = NULL;
static call_t *outgoing_calls = NULL;
static int incoming_calls_size = 0;
static int outgoing_calls_size = 0;



/* --- call management --- */

static void
_call_add(call_t ** calls, int *size, int id)
{
	g_debug("_call_add(%d)", id);
	(*size)++;
	if (*size == 1)
		*calls = malloc(sizeof(call_t));
	else
		*calls = realloc(*calls, sizeof(call_t) * (*size));
	(*calls)[(*size) - 1].id = id;
}

static int
_call_check(call_t * calls, int *size, int id)
{
	int i;
	g_debug("_call_check(%d)", id);
	for (i = 0; i < (*size); i++) {
		if (calls[i].id == id)
			return i;
	}
	return -1;
}

static void
_call_remove(call_t ** calls, int *size, int id)
{
	g_debug("_call_remove(%d)", id);
	if (*size == 1) {
		free(*calls);
		(*size)--;
		*calls = NULL;
	}
	else {
		int place = _call_check(*calls, size, id);
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


/* --- stuff happening before we have GSM resource up and running  --- */

static void
_list_resources_callback(GError *error, char **resources, gpointer userdata)
{
	/* if we successfully got a list of resources...
	 * check if GSM is within them and request it if
	 * so, otherwise wait for ResourceAvailable signal */
	if (error == NULL) {
		int i = 0;
		while (resources[i] != NULL) {
			g_debug("Resource %s available", resources[i]);
			if (!strcmp(resources[i], "GSM")) {
				gsm_available = TRUE;
				break;
			}
			i++;
		}

		if (gsm_available)
			ophonekitd_request_gsm();
	}
}

gboolean
ophonekitd_list_resources(void)
{
	ousaged_list_resources(_list_resources_callback, NULL);
	return (FALSE);
}


static void
_request_resource_callback(GError * error, gpointer userdata)
{
	g_debug("_request_resource_callback()");

	if (error == NULL) {
		/* nothing to do when there is no error
		 * the signal handler for ResourceChanged
		 * will do the rest */
		return;
	}

	/* we only request the GSM resource if it is actually
	 * available... if this does not work we retry it after
	 * some timeout ... */
	g_debug("request resource error, try again in 1s");
	g_debug("error: %s %s %d", error->message,
		g_quark_to_string(error->domain), error->code);
	g_timeout_add(1000, ophonekitd_request_gsm, NULL);
}

gboolean
ophonekitd_request_gsm(void)
{
	/* only request GSM if we know it is available */
	if (gsm_available) {
		g_debug("Request GSM resource");
		ousaged_request_resource("GSM", _request_resource_callback, NULL);
	}
	return (FALSE);
}


/* --- stuff happening after we successfully requested the GSM resource --- */

static void
_sim_ready_status_callback(GError * error, gboolean status, gpointer userdata)
{
	if (error) {
		g_debug("GetSimReady failed: %s %s %d", error->message,
			g_quark_to_string(error->domain), error->code);
		return;
	}

	/* if sim is already ready (by the ReadyStatus signal) - nothing to do */
	if (sim_ready)
		return;

	g_debug("_sim_ready_status_callback(status=%d)", status);
	if (status)
		ophonekitd_sim_ready_actions();
}

static void
_sim_auth_status_callback(GError * error, int status, gpointer userdata)
{
	g_debug("sim_auth_status_callback()");

	g_debug("sim_auth_active: %d", sim_auth_active);
	if (sim_auth_active) {
		sim_auth_active = FALSE;
		phoneuid_dialogs_hide_sim_auth(status);
	}

	if (status != SIM_READY) {
		if (!sim_auth_active) {
			sim_auth_active = TRUE;
			phoneuid_dialogs_show_sim_auth(status);
		}
		return;
	}

	ophonekitd_set_antenna_power();
	ogsmd_sim_get_sim_ready(_sim_ready_status_callback, NULL);
}

static void
_set_antenna_power_callback(GError * error, gpointer userdata)
{
	g_debug("_set_antenna_power_callback()");
	if (error != NULL) {
		if (IS_SIM_ERROR(error, SIM_ERROR_AUTH_FAILED)) {
			/*
			 * This auth status query is needed for startup when
			 * there's no auth status signal emitted
			 */
			if (!sim_auth_active) {
				ogsmd_sim_get_auth_status
					(_sim_auth_status_callback, NULL);
			}
			return;
		}
		else if (IS_SIM_ERROR(error, SIM_ERROR_NOT_PRESENT)) {
			g_message("SIM card not present.");
			phoneuid_dialogs_show_dialog(
				PHONEGUI_DIALOG_SIM_NOT_PRESENT);
			return;
		}
		else if (IS_RESOURCE_ERROR(error, RESOURCE_ERROR_NOT_ENABLED)) {
			g_message("GSM is not yet enabled, try again in 1s");
			g_timeout_add(1000, ophonekitd_set_antenna_power, NULL);
		}
		else if (IS_FRAMEWORKD_GLIB_DBUS_ERROR
			 (error,
			  FRAMEWORKD_GLIB_DBUS_ERROR_SERVICE_NOT_AVAILABLE)
			 || IS_FRAMEWORKD_GLIB_DBUS_ERROR(error,
							  FRAMEWORKD_GLIB_DBUS_ERROR_NO_REPLY)
			) {
			g_debug("dbus not available, try again in 5s");
			g_timeout_add(5000, ophonekitd_set_antenna_power, NULL);
			return;
		}
		else {
			g_debug("Unknown error: %s %s %d", error->message,
				g_quark_to_string(error->domain), error->code);
			g_timeout_add(5000, ophonekitd_set_antenna_power, NULL);
			return;
		}
	}
	ophonekitd_register_network();
}


gboolean
ophonekitd_set_antenna_power()
{
	g_debug("call ogsmd_device_set_antenna_power()");
	ogsmd_device_set_antenna_power(TRUE, _set_antenna_power_callback, NULL);
	return (FALSE);
}


static void
_register_to_network_callback(GError * error, gpointer userdata)
{
	g_debug("register_to_network_callback()");
	if (error == NULL) {
		g_debug("Successfully registered to the network");
	}
	else {
		g_debug("Registering to network failed: %s %s %d",
			error->message, g_quark_to_string(error->domain),
			error->code);
		/* when registering to the network fails we have to retry
		 * after some time again ... but only as long as GSM is
		 * enabled */
		if (gsm_ready) {
			/* gsm_reregister_timeout is in seconds ... */
			g_timeout_add(gsm_reregister_timeout * 1000,
				ophonekitd_register_network, NULL);
		}
	}
}

gboolean
ophonekitd_register_network(void)
{
	ogsmd_network_register(_register_to_network_callback, NULL);
	return (FALSE);
}


/* --- stuff happening when the SIM has ready status --- */

void
_get_messagebook_info_callback(GError * error, GHashTable * info,
			      gpointer userdata)
{
	g_debug("_get_messagebook_info_callback()");
	if (error == NULL && info != NULL) {
		gpointer p = NULL;
		int first = 0, last = 0, used = 0, total = 0;

		if ((p = g_hash_table_lookup(info, "first")) != NULL)
			first = g_value_get_int(p);
		else
			g_debug("get_messagebok_info_callback(): No value for \"first\"");

		if ((p = g_hash_table_lookup(info, "last")) != NULL)
			last = g_value_get_int(p);
		else
			g_debug("get_messagebok_info_callback(): No value for \"last\"");

		if ((p = g_hash_table_lookup(info, "used")) != NULL)
			used = g_value_get_int(p);
		else
			g_debug("get_messagebok_info_callback(): No value for \"used\"");

		total = last - first + 1;
		g_debug("messagebook info: first: %d, last %d, used: %d, total %d", first, last, used, total);
		if (used == total) {
			phoneuid_dialogs_show_dialog(
					PHONEGUI_DIALOG_MESSAGE_STORAGE_FULL);
		}
	}
	else {
		g_debug("MessageBookInfo failed: %s %s %d", error->message,
			g_quark_to_string(error->domain), error->code);
		/* TODO */
	}
	g_debug("_get_messagebook_info_callback done");
}




void
ophonekitd_sim_ready_actions(void)
{
	g_debug("sim ready");
	sim_ready = TRUE;
	ogsmd_sim_get_messagebook_info(_get_messagebook_info_callback, NULL);
}




/* === signal handlers === */

/* --- ResourceAvailable --- */

void
ophonekitd_resource_available_handler(const char *name, gboolean availability)
{
	g_debug("resource %s is now %s", name,
		availability ? "available" : "vanished");
	if (strcmp(name, "GSM") == 0) {
		gsm_available = availability;
		if (gsm_available) {
			/* TODO: airplane mode !!! */
			ophonekitd_request_gsm();
		}
	}
}

/* --- ResourceChanged --- */

void
ophonekitd_resource_changed_handler(const char *name, gboolean state,
				    GHashTable * attributes)
{
	gpointer p = NULL;
	g_debug("resource %s is now %s", name, state ? "enabled" : "disabled");
	p = g_hash_table_lookup(attributes, "policy");
	if (p)
		g_debug("   policy:   %d", g_value_get_int(p));
	p = g_hash_table_lookup(attributes, "refcount");
	if (p)
		g_debug("   refcount: %d", g_value_get_int(p));

	if (strcmp(name, "GSM") == 0) {
		/* check if state actually really changed for GSM */
		if (gsm_ready ^ state) {
			gsm_ready = state;
			if (gsm_ready) {
				/* TODO: remove this workaround when fsousaged
				 * sends the signal when the GSM resource
				 * actually is really ready - until then give
				 * it some time to get it ready ... */
				ophonekitd_set_antenna_power();
				ogsmd_sim_get_sim_ready
					(_sim_ready_status_callback, NULL);
			}
		}
	}
}


/* --- PowerState --- */

void
ophonekitd_device_idle_notifier_power_state_handler(GError * error,
						    const int status,
						    gpointer userdata)
{
	g_debug("power status: %d", status);
	if (incoming_calls_size == 0 && outgoing_calls_size == 0
	    && error == NULL && status != DEVICE_POWER_STATE_CHARGING
	    && status != DEVICE_POWER_STATE_FULL) {

		ousaged_suspend(NULL, NULL);
		g_debug("Suspend !");
	}
}

/* --- idle state --- */

void
ophonekitd_device_idle_notifier_state_handler(const int state)
{
	g_debug("idle notifier state handler called, id %d", state);

	if (state == DEVICE_IDLE_STATE_SUSPEND) {
		odeviced_power_supply_get_power_status(
			ophonekitd_device_idle_notifier_power_state_handler,
			NULL);
	}
}

/* --- CallStatus --- */
void
ophonekitd_call_status_handler(const int call_id, const int status,
			       GHashTable * properties)
{
	g_debug("call status handler called, id: %d, status: %d", call_id,
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
			g_debug("incoming call");
			if (_call_check(incoming_calls,
					&incoming_calls_size, call_id) == -1) {
				_call_add(&incoming_calls,
					&incoming_calls_size, call_id);
				phoneuid_call_management_show_incoming(
						call_id, status, number);
			}
			break;
		case CALL_STATUS_OUTGOING:
			g_debug("outgoing call");
			if (_call_check(outgoing_calls,
					&outgoing_calls_size, call_id) == -1) {
				_call_add(&outgoing_calls,
					&outgoing_calls_size, call_id);
				phoneuid_call_management_show_outgoing(
						call_id, status, number);
			}
			break;
		case CALL_STATUS_RELEASE:
			g_debug("release call");
			if (_call_check(incoming_calls,
					&incoming_calls_size, call_id) != -1) {
				_call_remove(&incoming_calls,
						&incoming_calls_size, call_id);
				phoneuid_call_management_hide_incoming(
						call_id);
			}
			if (_call_check(outgoing_calls,
					&outgoing_calls_size, call_id) != -1) {
				_call_remove(&outgoing_calls,
						&outgoing_calls_size, call_id);
				phoneuid_call_management_hide_outgoing(
						call_id);
			}
			break;
		case CALL_STATUS_HELD:
			g_debug("held call");
			break;
		case CALL_STATUS_ACTIVE:
			g_debug("active call");
			break;
		default:
			g_debug("Unknown CallStatus");
			break;
	}
}


/* --- AuthStatus --- */
void
ophonekitd_sim_auth_status_handler(const int status)
{
	g_debug("ophonekitd_sim_auth_status_handler()");
	if (status == SIM_READY) {
		g_debug("sim auth ready");

		if (sim_auth_active) {
			sim_auth_active = FALSE;
			phoneuid_dialogs_hide_sim_auth(status);
		}
		ophonekitd_set_antenna_power();
	}
	else {
		g_debug("sim not ready");
		if (!sim_auth_active) {
			sim_auth_active = TRUE;
			phoneuid_dialogs_show_sim_auth(status);
		}
	}
}


/* --- SimReady --- */

void
ophonekitd_sim_ready_status_handler(gboolean status)
{
	g_debug("ophonekitd_sim_ready_status_handler()");
	if (status)
		ophonekitd_sim_ready_actions();
}



/* --- IncomingStoredMessage --- */

void
ophonekitd_sim_incoming_stored_message_handler(const int id)
{
	g_debug("ophonekitd_sim_incoming_stored_message_handler()");
	if (show_incoming_sms == TRUE) {
		phoneuid_messages_display_item(id);
	}
	ogsmd_sim_get_messagebook_info(_get_messagebook_info_callback, NULL);
}

/* --- IncomingUssd --- */
void
ophonekitd_incoming_ussd_handler(int mode, const char *message)
{
	g_debug("ophonekitd_incoming_ussd_handler(mode=%d, message=%s)", mode,
		message);
	phoneuid_dialogs_show_ussd(mode, message);
}

/* --- NetworkStatus --- */
void
ophonekitd_network_status_handler(GHashTable *status)
{
	char *registration = g_value_get_string(
			g_hash_table_lookup(status, "registration"));
	g_debug("ophonekitd_network_status_handler(registration=%s)",
			registration);
	if (!strcmp(registration, "unregistered")) {
		g_message("scheduling registration to network");
		g_timeout_add(gsm_reregister_timeout * 1000,
				ophonekitd_register_network, NULL);
	}
}

