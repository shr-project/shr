#ifndef _PHONEUID_CONTACTS_H
#define _PHONEUID_CONTACTS_H

#define PHONEUID_TYPE_CONTACTS_SERVICE            (phoneuid_contacts_service_get_type ())
#define PHONEUID_CONTACTS_SERVICE(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), PHONEUID_TYPE_CONTACTS_SERVICE, PhoneuidContactsService))
#define PHONEUID_CONTACTS_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PHONEUID_TYPE_CONTACTS_SERVICE, PhoneuidContactsServiceClass))
#define PHONEUID_IS_CONTACTS_SERVICE(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), PHONEUID_TYPE_CONTACTS_SERVICE))
#define PHONEUID_IS_CONTACTS_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PHONEUID_TYPE_CONTACTS_SERVICE))
#define PHONEUID_CONTACTS_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), PHONEUID_TYPE_CONTACTS_SERVICE, PhoneuidContactsServiceClass))


typedef struct _PhoneuidContactsService PhoneuidContactsService;
typedef struct _PhoneuidContactsServiceClass PhoneuidContactsServiceClass;

GType phoneuid_contacts_service_get_type(void);

struct _PhoneuidContactsService {
	GObject parent;
};

struct _PhoneuidContactsServiceClass {
	GObjectClass parent;
};

gboolean
phoneuid_contacts_service_display_list(PhoneuidContactsService *object,
		GError **error);
gboolean
phoneuid_contacts_service_display_item(PhoneuidContactsService *object,
		const char *contact_path, GError **error);
gboolean
phoneuid_contacts_service_create_contact(PhoneuidContactsService *object,
		GHashTable *values, GError **error);
gboolean
phoneuid_contacts_service_edit_contact(PhoneuidContactsService *object,
		const char *contact_path, GError **error);



PhoneuidContactsService *phoneuid_contacts_service_new(void);



#endif
