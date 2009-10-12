
#include <glib.h>
#include <glib-object.h>
#include "ophonekitd-phonegui-contacts.h"
#include "ophonekitd-phonegui.h"

G_DEFINE_TYPE(OphonekitdContactsService, ophonekitd_contacts_service, G_TYPE_OBJECT)

static void
ophonekitd_contacts_service_class_init(OphonekitdContactsServiceClass *klass)
{
}

static void
ophonekitd_contacts_service_init(OphonekitdContactsService *object)
{
}


OphonekitdContactsService *
ophonekitd_contacts_service_new(void)
{
	return (g_object_new(OPHONEKITD_TYPE_CONTACTS_SERVICE, NULL));
}


/* === org.shr.ophonekitd.Contacts methods === */

/* --- DisplayList --- */

gboolean
ophonekitd_contacts_service_display_list(OphonekitdContactsService *object,
		GError **error)
{
	g_debug("org.shr.ophonekitd.Contacts.DisplayList");
	phonegui_contacts_show();
	return (TRUE);
}

gboolean
ophonekitd_contacts_service_display_item(OphonekitdContactsService *object,
		const char *contact_path, GError **error)
{
	g_debug("org.shr.ophonekitd.Contacts.DisplayItem");
	return (TRUE);
}

gboolean
ophonekitd_contacts_service_create_contact(OphonekitdContactsService *object,
		GHashTable *values, GError **error)
{
	g_debug("org.shr.ophonekitd.Contacts.CreateContact");
	return (TRUE);
}

gboolean
ophonekitd_contacts_service_edit_contact(OphonekitdContactsService *object,
		const char *contact_path, GError **error)
{
	g_debug("org.shr.ophonekitd.Contacts.EditContact");
	//dbus_g_method_return(context);
	return (TRUE);
}


