
#include <glib.h>
#include <glib-object.h>
#include <dbus/dbus-glib-bindings.h>
#include "phoneuid-dbus-common.h"
#include "phoneuid-dialogs.h"
#include "phoneuid-dialogs-service-glue.h"

G_DEFINE_TYPE(PhoneuidDialogsService, phoneuid_dialogs_service, G_TYPE_OBJECT)

static void
phoneuid_dialogs_service_class_init(PhoneuidDialogsServiceClass *klass)
{
	GError *error = NULL;

	/* Init the DBus connection, per-klass */
	klass->connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
	if (klass->connection == NULL) {
		g_warning("Unable to connect to dbus: %s", error->message);
		g_error_free (error);
		return;
	}

	dbus_g_object_type_install_info (PHONEUID_TYPE_DIALOGS_SERVICE,
			&dbus_glib_phoneuid_dialogs_service_object_info);
}

static void
phoneuid_dialogs_service_init(PhoneuidDialogsService *object)
{
	GError *error = NULL;
	DBusGProxy *driver_proxy;
	PhoneuidDialogsServiceClass *klass =
		PHONEUID_DIALOGS_SERVICE_GET_CLASS(object);
	int request_ret;

	/* Register DBUS path */
	dbus_g_connection_register_g_object(klass->connection,
			PHONEUID_DIALOGS_PATH,
			G_OBJECT (object));

	/* Register the service name, the constant here are defined in dbus-glib-bindings.h */
	driver_proxy = dbus_g_proxy_new_for_name (klass->connection,
			DBUS_SERVICE_DBUS,
			DBUS_PATH_DBUS,
			DBUS_INTERFACE_DBUS);

	if (!org_freedesktop_DBus_request_name (driver_proxy,
			PHONEUID_DIALOGS_NAME, 0, &request_ret, &error)) {
		g_warning("Unable to register service: %s", error->message);
		g_error_free (error);
	}
	g_object_unref(driver_proxy);
}


PhoneuidDialogsService *
phoneuid_dialogs_service_new(void)
{
	return (g_object_new(PHONEUID_TYPE_DIALOGS_SERVICE, NULL));
}


gboolean
phoneuid_dialogs_service_display_sim_auth(PhoneuidDialogsService *object,
		GError **error)
{
}

gboolean
phoneuid_dialogs_service_hide_sim_auth(PhoneuidDialogsService *object,
		GError **error)
{
}

gboolean
phoneuid_dialogs_service_display_ussd(PhoneuidDialogsService *object,
		const int mode, const char *message, GError **error)
{
}

gboolean
phoneuid_dialogs_service_display_dialog(PhoneuidDialogsService *object,
		const int dialog, GError **error)
{
}

