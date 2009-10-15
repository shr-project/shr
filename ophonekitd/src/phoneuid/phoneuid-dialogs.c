
#include <glib.h>
#include <glib-object.h>
#include "phoneuid-dialogs.h"

G_DEFINE_TYPE(PhoneuidDialogsService, phoneuid_dialogs_service, G_TYPE_OBJECT)

static void
phoneuid_dialogs_service_class_init(PhoneuidDialogsServiceClass *klass)
{
}

static void
phoneuid_dialogs_service_init(PhoneuidDialogsService *object)
{
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

