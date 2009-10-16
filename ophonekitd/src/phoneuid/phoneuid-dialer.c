
#include <glib.h>
#include <glib-object.h>
#include <dbus/dbus-glib-bindings.h>
#include "phoneuid-dbus-common.h"
#include "phoneuid-dialer.h"
#include "phoneuid-dialer-service-glue.h"

G_DEFINE_TYPE(PhoneuidDialerService, phoneuid_dialer_service, G_TYPE_OBJECT)

static void
phoneuid_dialer_service_class_init(PhoneuidDialerServiceClass *klass)
{
	GError *error = NULL;

	/* Init the DBus connection, per-klass */
	klass->connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
	if (klass->connection == NULL) {
		g_warning("Unable to connect to dbus: %s", error->message);
		g_error_free (error);
		return;
	}

	dbus_g_object_type_install_info (PHONEUID_TYPE_DIALER_SERVICE,
			&dbus_glib_phoneuid_dialer_service_object_info);
}

static void
phoneuid_dialer_service_init(PhoneuidDialerService *object)
{
	GError *error = NULL;
	DBusGProxy *driver_proxy;
	PhoneuidDialerServiceClass *klass =
		PHONEUID_DIALER_SERVICE_GET_CLASS(object);
	int request_ret;

	/* Register DBUS path */
	dbus_g_connection_register_g_object(klass->connection,
			PHONEUID_DIALER_PATH,
			G_OBJECT (object));

	/* Register the service name, the constant here are defined in dbus-glib-bindings.h */
	driver_proxy = dbus_g_proxy_new_for_name (klass->connection,
			DBUS_SERVICE_DBUS,
			DBUS_PATH_DBUS,
			DBUS_INTERFACE_DBUS);

	if (!org_freedesktop_DBus_request_name (driver_proxy,
			PHONEUID_DIALER_NAME, 0, &request_ret, &error)) {
		g_warning("Unable to register service: %s", error->message);
		g_error_free (error);
	}
	g_object_unref(driver_proxy);
}


PhoneuidDialerService *
phoneuid_dialer_service_new(void)
{
	return (g_object_new(PHONEUID_TYPE_DIALER_SERVICE, NULL));
}



gboolean
phoneuid_dialer_service_display(PhoneuidDialerService *object,
		GHashTable *options, DBusGMethodInvocation *context)
{
	dbus_g_method_return(context);
	phonegui_dialer_show();
	return (TRUE);
}

