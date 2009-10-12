
#include <glib.h>
#include <glib-object.h>
#include "phoneuid-contacts.h"
#include "phoneuid.h"

G_DEFINE_TYPE(PhoneuidContactsService, phoneuid_contacts_service, G_TYPE_OBJECT)

static void
phoneuid_contacts_service_class_init(PhoneuidContactsServiceClass *klass)
{
}

static void
phoneuid_contacts_service_init(PhoneuidContactsService *object)
{
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


