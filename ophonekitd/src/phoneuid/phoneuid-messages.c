
#include <glib.h>
#include <glib-object.h>
#include "phoneuid-messages.h"
#include "phoneuid.h"

G_DEFINE_TYPE(PhoneuidMessagesService, phoneuid_messages_service, G_TYPE_OBJECT)

static void
phoneuid_messages_service_class_init(PhoneuidMessagesServiceClass *klass)
{
}

static void
phoneuid_messages_service_init(PhoneuidMessagesService *object)
{
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

