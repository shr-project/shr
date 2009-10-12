#ifndef _OPHONEKITD_PHONEGUI_CALL_MANAGEMENT_H
#define _OPHONEKITD_PHONEGUI_CALL_MANAGEMENT_H

#define OPHONEKITD_TYPE_CALL_MANAGEMENT_SERVICE            (ophonekitd_call_management_service_get_type ())
#define OPHONEKITD_CALL_MANAGEMENT_SERVICE(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), OPHONEKITD_TYPE_CALL_MANAGEMENT_SERVICE, OphonekitdCallManagementService))
#define OPHONEKITD_CALL_MANAGEMENT_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), OPHONEKITD_TYPE_CALL_MANAGEMENT_SERVICE, OphonekitdCallManagementServiceClass))
#define OPHONEKITD_IS_CALL_MANAGEMENT_SERVICE(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), OPHONEKITD_TYPE_CALL_MANAGEMENT_SERVICE))
#define OPHONEKITD_IS_CALL_MANAGEMENT_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), OPHONEKITD_TYPE_CALL_MANAGEMENT_SERVICE))
#define OPHONEKITD_CALL_MANAGEMENT_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), OPHONEKITD_TYPE_CALL_MANAGEMENT_SERVICE, OphonekitdCallManagementServiceClass))

typedef struct _OphonekitdCallManagementService OphonekitdCallManagementService;
typedef struct _OphonekitdCallManagementServiceClass OphonekitdCallManagementServiceClass;

GType ophonekitd_contacts_service_get_type(void);

struct _OphonekitdCallManagementService {
	GObject parent;
};

struct _OphonekitdCallManagementServiceClass {
	GObjectClass parent;
};

gboolean
ophonekitd_call_management_service_display_incoming(
		OphonekitdCallManagementService *object, const int call_id,
		const int status, const char *number, GError **error);
gboolean
ophonekitd_call_management_service_hide_incoming(
		OphonekitdCallManagementService *object, const int call_id,
		GError **error);

gboolean
ophonekitd_call_management_service_display_outgoing(
		OphonekitdCallManagementService *object, const int call_id,
		const int status, const char *number, GError **error);

gboolean
ophonekitd_call_management_service_hide_outgoing(
		OphonekitdCallManagementService *object, const int call_id,
		GError **error);


OphonekitdCallManagementService *ophonekitd_call_management_service_new(void);

#endif
