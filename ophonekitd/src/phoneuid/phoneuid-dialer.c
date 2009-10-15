
#include <glib.h>
#include <glib-object.h>
#include <dbus/dbus-glib-bindings.h>
#include "phoneuid-dialer.h"

G_DEFINE_TYPE(PhoneuidDialerService, phoneuid_dialer_service, G_TYPE_OBJECT)

static void
phoneuid_dialer_service_class_init(PhoneuidDialerServiceClass *klass)
{
}

static void
phoneuid_dialer_service_init(PhoneuidDialerService *object)
{
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

