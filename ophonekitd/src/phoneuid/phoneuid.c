
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <frameworkd-phonegui/frameworkd-phonegui.h>
#include "phoneuid.h"
#include "phoneuid-contacts.h"
#include "phoneuid-messages.h"
#include "phoneuid-call-management.h"
#include "phoneuid-dbus-common.h"
#include "phoneuid-contacts-service-glue.h"
#include "phoneuid-messages-service-glue.h"
#include "phoneuid-call-management-service-glue.h"


int
main(int argc, char **argv)
{
	g_type_init();

	if (!g_thread_supported())
		g_thread_init(NULL);
	dbus_g_thread_init();

	phonegui_load("phoneuid");
	phonegui_init(argc, argv, NULL);
	phoneuid_dbus_setup();
	phonegui_loop();
	g_debug("exited from phonegui_loop!");
}


static gpointer
dbus_register_object(DBusGConnection * connection,
		     DBusGProxy * proxy,
		     GType object_type,
		     const DBusGObjectInfo * info, const gchar * path)
{
	g_debug(path);
	GObject *object = g_object_new(object_type, NULL);
	dbus_g_object_type_install_info(object_type, info);
	dbus_g_connection_register_g_object(connection, path, object);
	return object;
}



void
phoneuid_dbus_setup()
{
	GError *error = NULL;
	guint result;
	DBusGConnection *connection;
	DBusGProxy *proxy;

	connection = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
	if (error) {
		g_error("%d: %s", error->code, error->message);
	}
	proxy = dbus_g_proxy_new_for_name(connection,
			DBUS_SERVICE_DBUS,
			DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);

	/* -- register org.shr.phoneuid.CallManagement -- */
	if (!org_freedesktop_DBus_request_name(proxy,
				PHONEUID_CALL_MANAGEMENT_SERVICE_NAME,
				DBUS_NAME_FLAG_DO_NOT_QUEUE,
				&result, &error)) {
		g_error("Error requesting name: %d %s",
				error->code, error->message);
	}

	if (result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
		g_error("Got result code %u from requesting %s", result,
				PHONEUID_CALL_MANAGEMENT_SERVICE_NAME);
	}

	dbus_register_object(connection, proxy,
			PHONEUID_TYPE_CALL_MANAGEMENT_SERVICE,
			&dbus_glib_phoneuid_call_management_service_object_info,
			PHONEUID_CALL_MANAGEMENT_SERVICE_PATH);

	/* -- register org.shr.phoneuid.Contacts -- */
	if (!org_freedesktop_DBus_request_name(proxy,
				PHONEUID_CONTACTS_SERVICE_NAME,
				DBUS_NAME_FLAG_DO_NOT_QUEUE,
				&result, &error)) {
		g_error("Error requesting name: %d %s",
				error->code, error->message);
	}

	if (result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
		g_error("Got result code %u from requesting %s", result,
				PHONEUID_CONTACTS_SERVICE_NAME);
	}

	dbus_register_object(connection, proxy,
			PHONEUID_TYPE_CONTACTS_SERVICE,
			&dbus_glib_phoneuid_contacts_service_object_info,
			PHONEUID_CONTACTS_SERVICE_PATH);

}

