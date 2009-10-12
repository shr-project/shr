
#include <glib.h>
#include <glib-object.h>
#include "ophonekitd-phonegui-messages.h"
#include "ophonekitd-phonegui.h"

G_DEFINE_TYPE(OphonekitdMessagesService, ophonekitd_messages_service, G_TYPE_OBJECT)

static void
ophonekitd_messages_service_class_init(OphonekitdMessagesServiceClass *klass)
{
}

static void
ophonekitd_messages_service_init(OphonekitdMessagesService *object)
{
}


OphonekitdMessagesService *
ophonekitd_messages_service_new(void)
{
	return (g_object_new(OPHONEKITD_TYPE_MESSAGES_SERVICE, NULL));
}


gboolean
ophonekitd_messages_service_display_list(OphonekitdMessagesService *object,
		GError **error)
{
	g_debug("org.shr.ophonekitd.Messages.DisplayList");
	phonegui_messages_show();
	return (TRUE);
}

gboolean
ophonekitd_messages_service_display_item(OphonekitdMessagesService *object,
		const char *message_path, GError **error)
{
	g_debug("org.shr.ophonekitd.Contacts.DisplayItem");
	return (TRUE);
}

gboolean
ophonekitd_messages_service_create_message(OphonekitdMessagesService *object,
		GHashTable *values, GError **error)
{
	g_debug("org.shr.ophonekitd.Contacts.CreateContact");
	return (TRUE);
}

