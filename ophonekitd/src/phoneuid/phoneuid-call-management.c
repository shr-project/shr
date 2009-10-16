

#include <glib.h>
#include <glib-object.h>
#include <dbus/dbus-glib-bindings.h>
#include "phoneuid-dbus-common.h"
#include "phoneuid-call-management.h"
#include "phoneuid-call-management-service-glue.h"

G_DEFINE_TYPE(PhoneuidCallManagementService, phoneuid_call_management_service, G_TYPE_OBJECT)

static void
phoneuid_call_management_service_class_init(PhoneuidCallManagementServiceClass *klass)
{
	GError *error = NULL;

	/* Init the DBus connection, per-klass */
	klass->connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
	if (klass->connection == NULL) {
		g_warning("Unable to connect to dbus: %s", error->message);
		g_error_free (error);
		return;
	}

	dbus_g_object_type_install_info (PHONEUID_TYPE_CALL_MANAGEMENT_SERVICE,
			&dbus_glib_phoneuid_call_management_service_object_info);
}

static void
phoneuid_call_management_service_init(PhoneuidCallManagementService *object)
{
	GError *error = NULL;
	DBusGProxy *driver_proxy;
	PhoneuidCallManagementServiceClass *klass =
		PHONEUID_CALL_MANAGEMENT_SERVICE_GET_CLASS(object);
	int request_ret;

	/* Register DBUS path */
	dbus_g_connection_register_g_object(klass->connection,
			PHONEUID_CALL_MANAGEMENT_PATH,
			G_OBJECT (object));

	/* Register the service name, the constant here are defined in dbus-glib-bindings.h */
	driver_proxy = dbus_g_proxy_new_for_name (klass->connection,
			DBUS_SERVICE_DBUS,
			DBUS_PATH_DBUS,
			DBUS_INTERFACE_DBUS);

	if (!org_freedesktop_DBus_request_name (driver_proxy,
			PHONEUID_CALL_MANAGEMENT_NAME, 0, &request_ret, &error)) {
		g_warning("Unable to register service: %s", error->message);
		g_error_free (error);
	}
	g_object_unref(driver_proxy);
}


PhoneuidCallManagementService *
phoneuid_call_management_service_new(void)
{
	return (g_object_new(PHONEUID_TYPE_CALL_MANAGEMENT_SERVICE, NULL));
}

gboolean
phoneuid_call_management_service_display_incoming(
		PhoneuidCallManagementService *object, const int call_id,
		const int status, const char *number, GError **error)
{
	g_debug("org.shr.phoneuid.CallManagement.DisplayIncoming");
	phonegui_incoming_call_show(call_id, status, number);
	return (TRUE);
}

gboolean
phoneuid_call_management_service_hide_incoming(
		PhoneuidCallManagementService *object, const gint call_id,
		GError **error)
{
	g_debug("org.shr.phoneuid.CallManagement.HideIncoming");
	phonegui_incoming_call_hide(call_id);
	return (TRUE);
}


gboolean
phoneuid_call_management_service_display_outgoing(
		PhoneuidCallManagementService *object, const int call_id,
		const int status, const char *number, GError **error)
{
	g_debug("org.shr.phoneuid.CallManagement.DisplayOutgoing");
	phonegui_outgoing_call_show(call_id, status, number);
	return (TRUE);
}

gboolean
phoneuid_call_management_service_hide_outgoing(
		PhoneuidCallManagementService *object, const int call_id,
		GError **error)
{
	g_debug("org.shr.phoneuid.CallManagement.HideOutgoing");
	phonegui_outgoing_call_hide(call_id);
	return (TRUE);
}
