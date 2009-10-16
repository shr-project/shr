#ifndef _PHONEUID_CALL_MANAGEMENT_H
#define _PHONEUID_CALL_MANAGEMENT_H

#define PHONEUID_TYPE_CALL_MANAGEMENT_SERVICE            (phoneuid_call_management_service_get_type ())
#define PHONEUID_CALL_MANAGEMENT_SERVICE(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), PHONEUID_TYPE_CALL_MANAGEMENT_SERVICE, PhoneuidCallManagementService))
#define PHONEUID_CALL_MANAGEMENT_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PHONEUID_TYPE_CALL_MANAGEMENT_SERVICE, PhoneuidCallManagementServiceClass))
#define PHONEUID_IS_CALL_MANAGEMENT_SERVICE(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), PHONEUID_TYPE_CALL_MANAGEMENT_SERVICE))
#define PHONEUID_IS_CALL_MANAGEMENT_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PHONEUID_TYPE_CALL_MANAGEMENT_SERVICE))
#define PHONEUID_CALL_MANAGEMENT_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), PHONEUID_TYPE_CALL_MANAGEMENT_SERVICE, PhoneuidCallManagementServiceClass))

typedef struct _PhoneuidCallManagementService PhoneuidCallManagementService;
typedef struct _PhoneuidCallManagementServiceClass PhoneuidCallManagementServiceClass;

GType phoneuid_contacts_service_get_type(void);

struct _PhoneuidCallManagementService {
	GObject parent;
};

struct _PhoneuidCallManagementServiceClass {
	GObjectClass parent;
	DBusGConnection *connection;
};

gboolean
phoneuid_call_management_service_display_incoming(
		PhoneuidCallManagementService *object, const int call_id,
		const int status, const char *number, GError **error);
gboolean
phoneuid_call_management_service_hide_incoming(
		PhoneuidCallManagementService *object, const int call_id,
		GError **error);

gboolean
phoneuid_call_management_service_display_outgoing(
		PhoneuidCallManagementService *object, const int call_id,
		const int status, const char *number, GError **error);

gboolean
phoneuid_call_management_service_hide_outgoing(
		PhoneuidCallManagementService *object, const int call_id,
		GError **error);


PhoneuidCallManagementService *phoneuid_call_management_service_new(void);

#endif
