

#include <glib.h>
#include <glib-object.h>
#include "phoneuid-call-management.h"
#include "phoneuid.h"

G_DEFINE_TYPE(PhoneuidCallManagementService, phoneuid_call_management_service, G_TYPE_OBJECT)

static void
phoneuid_call_management_service_class_init(PhoneuidCallManagementServiceClass *klass)
{
}

static void
phoneuid_call_management_service_init(PhoneuidCallManagementService *object)
{
}


PhoneuidCallManagementService *
phoneuid_call_management_service_new(void)
{
	return (g_object_new(PHONEUID_TYPE_CALL_MANAGEMENT_SERVICE, NULL));
}

gboolean
phoneuid_call_management_service_display_incoming(
		PhoneuidCallManagementService *object, const int call_id,
		const int status, const char *number, GError **error)
{
	g_debug("org.shr.phoneuid.CallManagement.DisplayIncoming");
	phonegui_incoming_call_show(call_id, status, number);
	return (TRUE);
}

gboolean
phoneuid_call_management_service_hide_incoming(
		PhoneuidCallManagementService *object, const gint call_id,
		GError **error)
{
	g_debug("org.shr.phoneuid.CallManagement.HideIncoming");
	phonegui_incoming_call_hide(call_id);
	return (TRUE);
}


gboolean
phoneuid_call_management_service_display_outgoing(
		PhoneuidCallManagementService *object, const int call_id,
		const int status, const char *number, GError **error)
{
	g_debug("org.shr.phoneuid.CallManagement.DisplayOutgoing");
	phonegui_outgoing_call_show(call_id, status, number);
	return (TRUE);
}

gboolean
phoneuid_call_management_service_hide_outgoing(
		PhoneuidCallManagementService *object, const int call_id,
		GError **error)
{
	g_debug("org.shr.phoneuid.CallManagement.HideOutgoing");
	phonegui_outgoing_call_hide(call_id);
	return (TRUE);
}
