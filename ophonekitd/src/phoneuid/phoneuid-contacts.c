
#include <glib.h>
#include <glib-object.h>
#include <dbus/dbus-glib-bindings.h>
#include "phoneuid-dbus-common.h"
#include "phoneuid-contacts.h"
#include "phoneuid-contacts-service-glue.h"

G_DEFINE_TYPE(PhoneuidContactsService, phoneuid_contacts_service, G_TYPE_OBJECT)

static void
phoneuid_contacts_service_class_init(PhoneuidContactsServiceClass *klass)
{
	GError *error = NULL;

	/* Init the DBus connection, per-klass */
	klass->connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
	if (klass->connection == NULL) {
		g_warning("Unable to connect to dbus: %s", error->message);
		g_error_free (error);
		return;
	}

	dbus_g_object_type_install_info (PHONEUID_TYPE_CONTACTS_SERVICE,
			&dbus_glib_phoneuid_contacts_service_object_info);
}

static void
phoneuid_contacts_service_init(PhoneuidContactsService *object)
{
	GError *error = NULL;
	DBusGProxy *driver_proxy;
	PhoneuidContactsServiceClass *klass =
		PHONEUID_CONTACTS_SERVICE_GET_CLASS(object);
	int request_ret;

	/* Register DBUS path */
	dbus_g_connection_register_g_object(klass->connection,
			PHONEUID_CONTACTS_PATH,
			G_OBJECT (object));

	/* Register the service name, the constant here are defined in dbus-glib-bindings.h */
	driver_proxy = dbus_g_proxy_new_for_name (klass->connection,
			DBUS_SERVICE_DBUS,
			DBUS_PATH_DBUS,
			DBUS_INTERFACE_DBUS);

	if (!org_freedesktop_DBus_request_name (driver_proxy,
			PHONEUID_CONTACTS_NAME, 0, &request_ret, &error)) {
		g_warning("Unable to register service: %s", error->message);
		g_error_free (error);
	}
	g_object_unref(driver_proxy);
}


PhoneuidContactsService *
phoneuid_contacts_service_new(void)
{
	return (g_object_new(PHONEUID_TYPE_CONTACTS_SERVICE, NULL));
}


/* === org.shr.phoneuid.Contacts methods === */

/* --- DisplayList --- */

gboolean
phoneuid_contacts_service_display_list(PhoneuidContactsService *object,
		GError **error)
{
	g_debug("org.shr.phoneuid.Contacts.DisplayList");
	phonegui_contacts_show();
	return (TRUE);
}

gboolean
phoneuid_contacts_service_display_item(PhoneuidContactsService *object,
		const char *contact_path, GError **error)
{
	g_debug("org.shr.phoneuid.Contacts.DisplayItem");
	return (TRUE);
}

gboolean
phoneuid_contacts_service_create_contact(PhoneuidContactsService *object,
		GHashTable *values, GError **error)
{
	g_debug("org.shr.phoneuid.Contacts.CreateContact");
	return (TRUE);
}

gboolean
phoneuid_contacts_service_edit_contact(PhoneuidContactsService *object,
		const char *contact_path, GError **error)
{
	g_debug("org.shr.phoneuid.Contacts.EditContact");
	//dbus_g_method_return(context);
	return (TRUE);
}


