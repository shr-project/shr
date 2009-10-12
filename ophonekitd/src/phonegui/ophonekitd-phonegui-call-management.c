

#include <glib.h>
#include <glib-object.h>
#include "ophonekitd-phonegui-call-management.h"
#include "ophonekitd-phonegui.h"

G_DEFINE_TYPE(OphonekitdCallManagementService, ophonekitd_call_management_service, G_TYPE_OBJECT)

static void
ophonekitd_call_management_service_class_init(OphonekitdCallManagementServiceClass *klass)
{
}

static void
ophonekitd_call_management_service_init(OphonekitdCallManagementService *object)
{
}


OphonekitdCallManagementService *
ophonekitd_call_management_service_new(void)
{
	return (g_object_new(OPHONEKITD_TYPE_CALL_MANAGEMENT_SERVICE, NULL));
}

gboolean
ophonekitd_call_management_service_display_incoming(
		OphonekitdCallManagementService *object, const int call_id,
		const int status, const char *number, GError **error)
{
	g_debug("org.shr.ophonekitd.CallManagement.DisplayIncoming");
	phonegui_incoming_call_show(call_id, status, number);
	return (TRUE);
}

gboolean
ophonekitd_call_management_service_hide_incoming(
		OphonekitdCallManagementService *object, const gint call_id,
		GError **error)
{
	g_debug("org.shr.ophonekitd.CallManagement.HideIncoming");
	phonegui_incoming_call_hide(call_id);
	return (TRUE);
}


gboolean
ophonekitd_call_management_service_display_outgoing(
		OphonekitdCallManagementService *object, const int call_id,
		const int status, const char *number, GError **error)
{
	g_debug("org.shr.ophonekitd.CallManagement.DisplayOutgoing");
	phonegui_outgoing_call_show(call_id, status, number);
	return (TRUE);
}

gboolean
ophonekitd_call_management_service_hide_outgoing(
		OphonekitdCallManagementService *object, const int call_id,
		GError **error)
{
	g_debug("org.shr.ophonekitd.CallManagement.HideOutgoing");
	phonegui_outgoing_call_hide(call_id);
	return (TRUE);
}
