
#include <glib.h>
#include <glib-object.h>
#include <dbus/dbus-glib-bindings.h>
#include "phoneuid-dbus-common.h"
#include "phoneuid-messages.h"
#include "phoneuid-messages-service-glue.h"

G_DEFINE_TYPE(PhoneuidMessagesService, phoneuid_messages_service, G_TYPE_OBJECT)

static void
phoneuid_messages_service_class_init(PhoneuidMessagesServiceClass *klass)
{
	GError *error = NULL;

	/* Init the DBus connection, per-klass */
	klass->connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
	if (klass->connection == NULL) {
		g_warning("Unable to connect to dbus: %s", error->message);
		g_error_free (error);
		return;
	}

	dbus_g_object_type_install_info (PHONEUID_TYPE_MESSAGES_SERVICE,
			&dbus_glib_phoneuid_messages_service_object_info);
}

static void
phoneuid_messages_service_init(PhoneuidMessagesService *object)
{
	GError *error = NULL;
	DBusGProxy *driver_proxy;
	PhoneuidMessagesServiceClass *klass = 
		PHONEUID_MESSAGES_SERVICE_GET_CLASS(object);
	int request_ret;

	/* Register DBUS path */
	dbus_g_connection_register_g_object(klass->connection,
			PHONEUID_MESSAGES_PATH,
			G_OBJECT (object));

	/* Register the service name, the constant here are defined in dbus-glib-bindings.h */
	driver_proxy = dbus_g_proxy_new_for_name (klass->connection,
			DBUS_SERVICE_DBUS,
			DBUS_PATH_DBUS,
			DBUS_INTERFACE_DBUS);

	if (!org_freedesktop_DBus_request_name (driver_proxy,
			PHONEUID_MESSAGES_NAME, 0, &request_ret, &error)) {
		g_warning("Unable to register service: %s", error->message);
		g_error_free (error);
	}
	g_object_unref(driver_proxy);
}


PhoneuidMessagesService *
phoneuid_messages_service_new(void)
{
	return (g_object_new(PHONEUID_TYPE_MESSAGES_SERVICE, NULL));
}


gboolean
phoneuid_messages_service_display_list(PhoneuidMessagesService *object,
		GHashTable *options, DBusGMethodInvocation *context)
{
	g_debug("org.shr.phoneuid.Messages.DisplayList");
	dbus_g_method_return(context);
	phonegui_messages_show();
	return (TRUE);
}

gboolean
phoneuid_messages_service_display_item(PhoneuidMessagesService *object,
		const char *message_path, GError **error)
{
	g_debug("org.shr.phoneuid.Contacts.DisplayItem");
	return (TRUE);
}

gboolean
phoneuid_messages_service_create_message(PhoneuidMessagesService *object,
		GHashTable *values, GError **error)
{
	g_debug("org.shr.phoneuid.Contacts.CreateContact");
	return (TRUE);
}

