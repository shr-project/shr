
#include <string.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include "ophonekitd-phonegui-dbus.h"
#include "ophonekitd-globals.h"
#include "../phonegui/ophonekitd-phonegui-dbus-common.h"


static void
_dbus_error(GError *error)
{
	if (error->domain == DBUS_GERROR &&
		error->code == DBUS_GERROR_REMOTE_EXCEPTION) {
		g_message("Remote method exception %s: %s",
			dbus_g_error_get_name (error),
			error->message);
	}
	else {
		g_message("Error: %s", error->message);
	}
	g_error_free(error);
}

/* --- org.shr.ophonekitd.CallManagement --- */
void phonegui_call_management_show_incoming(int callid, int status,
		const char *number)
{
	GError *error = NULL;
	DBusGProxy *proxy = dbus_g_proxy_new_for_name(system_bus,
			OPHONEKITD_CALL_MANAGEMENT_SERVICE_NAME,
			OPHONEKITD_CALL_MANAGEMENT_SERVICE_PATH,
			OPHONEKITD_CALL_MANAGEMENT_SERVICE_NAME);
	if (!dbus_g_proxy_call (proxy, "DisplayIncoming", &error,
			G_TYPE_INT, callid, G_TYPE_INT, status,
			G_TYPE_STRING, number, G_TYPE_INVALID,
			G_TYPE_INVALID))
		_dbus_error(error);
}

void phonegui_call_management_hide_incoming(int callid)
{
	GError *error = NULL;
	DBusGProxy *proxy = dbus_g_proxy_new_for_name(system_bus,
			OPHONEKITD_CALL_MANAGEMENT_SERVICE_NAME,
			OPHONEKITD_CALL_MANAGEMENT_SERVICE_PATH,
			OPHONEKITD_CALL_MANAGEMENT_SERVICE_NAME);
	if (!dbus_g_proxy_call (proxy, "HideIncoming", &error,
			G_TYPE_INT, callid, G_TYPE_INVALID,
			G_TYPE_INVALID))
		_dbus_error(error);
}

void
phonegui_call_management_show_outgoing(int callid, int status,
		const char *number)
{
	GError *error = NULL;
	DBusGProxy *proxy = dbus_g_proxy_new_for_name(system_bus,
			OPHONEKITD_CALL_MANAGEMENT_SERVICE_NAME,
			OPHONEKITD_CALL_MANAGEMENT_SERVICE_PATH,
			OPHONEKITD_CALL_MANAGEMENT_SERVICE_NAME);
	if (!dbus_g_proxy_call (proxy, "DisplayOutgoing", &error,
			G_TYPE_INT, callid, G_TYPE_INT, status,
			G_TYPE_STRING, number, G_TYPE_INVALID,
			G_TYPE_INVALID))
		_dbus_error(error);
}

void
phonegui_call_management_hide_outgoing(int callid)
{
	GError *error = NULL;
	DBusGProxy *proxy = dbus_g_proxy_new_for_name(system_bus,
			OPHONEKITD_CALL_MANAGEMENT_SERVICE_NAME,
			OPHONEKITD_CALL_MANAGEMENT_SERVICE_PATH,
			OPHONEKITD_CALL_MANAGEMENT_SERVICE_NAME);
	if (!dbus_g_proxy_call (proxy, "HideOutgoing", &error,
			G_TYPE_INT, callid, G_TYPE_INVALID,
			G_TYPE_INVALID))
		_dbus_error(error);
}

/* --- org.shr.ophonekitd.Messages --- */
void
phonegui_messages_display_item(const char *message_path)
{
	GError *error = NULL;
	DBusGProxy *proxy = dbus_g_proxy_new_for_name(system_bus,
			OPHONEKITD_MESSAGES_SERVICE_NAME,
			OPHONEKITD_MESSAGES_SERVICE_PATH,
			OPHONEKITD_MESSAGES_SERVICE_NAME);
	if (!dbus_g_proxy_call (proxy, "DisplayItem", &error,
			G_TYPE_STRING, message_path, G_TYPE_INVALID,
			G_TYPE_INVALID))
		_dbus_error(error);
}

/* --- org.shr.ophonekitd.Dialogs --- */
void
phonegui_dialogs_show_dialog(int dialog)
{
}

void
phonegui_dialogs_show_sim_auth(int status)
{
}

void
phonegui_dialogs_hide_sim_auth(int status)
{
}

void
phonegui_dialogs_show_ussd(int mode, const char *message)
{
}


