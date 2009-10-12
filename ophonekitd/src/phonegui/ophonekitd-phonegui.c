
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <frameworkd-phonegui/frameworkd-phonegui.h>
#include "ophonekitd-phonegui.h"
#include "ophonekitd-phonegui-contacts.h"
#include "ophonekitd-phonegui-call-management.h"
#include "ophonekitd-phonegui-dbus-common.h"
#include "ophonekitd-contacts-service-glue.h"
#include "ophonekitd-call-management-service-glue.h"


int
main(int argc, char **argv)
{
	g_type_init();

	if (!g_thread_supported())
		g_thread_init(NULL);
	dbus_g_thread_init();

	phonegui_load("ophonekitd");
	phonegui_init(argc, argv, NULL);
	phonegui_dbus_setup();
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
phonegui_dbus_setup()
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

	/* -- register org.shr.ophonekitd.CallManagement -- */
	if (!org_freedesktop_DBus_request_name(proxy,
				OPHONEKITD_CALL_MANAGEMENT_SERVICE_NAME,
				DBUS_NAME_FLAG_DO_NOT_QUEUE,
				&result, &error)) {
		g_error("Error requesting name: %d %s",
				error->code, error->message);
	}

	if (result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
		g_error("Got result code %u from requesting %s", result,
				OPHONEKITD_CALL_MANAGEMENT_SERVICE_NAME);
	}

	dbus_register_object(connection, proxy,
			OPHONEKITD_TYPE_CALL_MANAGEMENT_SERVICE,
			&dbus_glib_ophonekitd_call_management_service_object_info,
			OPHONEKITD_CALL_MANAGEMENT_SERVICE_PATH);

	/* -- register org.shr.ophonekitd.Contacts -- */
	if (!org_freedesktop_DBus_request_name(proxy,
				OPHONEKITD_CONTACTS_SERVICE_NAME,
				DBUS_NAME_FLAG_DO_NOT_QUEUE,
				&result, &error)) {
		g_error("Error requesting name: %d %s",
				error->code, error->message);
	}

	if (result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
		g_error("Got result code %u from requesting %s", result,
				OPHONEKITD_CONTACTS_SERVICE_NAME);
	}

	dbus_register_object(connection, proxy,
			OPHONEKITD_TYPE_CONTACTS_SERVICE,
			&dbus_glib_ophonekitd_contacts_service_object_info,
			OPHONEKITD_CONTACTS_SERVICE_PATH);

}

