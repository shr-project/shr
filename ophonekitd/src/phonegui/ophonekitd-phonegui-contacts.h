#ifndef _OPHONEKITD_PHONEGUI_CONTACTS_H
#define _OPHONEKITD_PHONEGUI_CONTACTS_H

#define OPHONEKITD_TYPE_CONTACTS_SERVICE            (ophonekitd_contacts_service_get_type ())
#define OPHONEKITD_CONTACTS_SERVICE(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), OPHONEKITD_TYPE_CONTACTS_SERVICE, OphonekitdContactsService))
#define OPHONEKITD_CONTACTS_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), OPHONEKITD_TYPE_CONTACTS_SERVICE, OphonekitdContactsServiceClass))
#define OPHONEKITD_IS_CONTACTS_SERVICE(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), OPHONEKITD_TYPE_CONTACTS_SERVICE))
#define OPHONEKITD_IS_CONTACTS_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), OPHONEKITD_TYPE_CONTACTS_SERVICE))
#define OPHONEKITD_CONTACTS_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), OPHONEKITD_TYPE_CONTACTS_SERVICE, OphonekitdContactsServiceClass))


typedef struct _OphonekitdContactsService OphonekitdContactsService;
typedef struct _OphonekitdContactsServiceClass OphonekitdContactsServiceClass;

GType ophonekitd_contacts_service_get_type(void);

struct _OphonekitdContactsService {
	GObject parent;
};

struct _OphonekitdContactsServiceClass {
	GObjectClass parent;
};

gboolean
ophonekitd_contacts_service_display_list(OphonekitdContactsService *object,
		GError **error);
gboolean
ophonekitd_contacts_service_display_item(OphonekitdContactsService *object,
		const char *contact_path, GError **error);
gboolean
ophonekitd_contacts_service_create_contact(OphonekitdContactsService *object,
		GHashTable *values, GError **error);
gboolean
ophonekitd_contacts_service_edit_contact(OphonekitdContactsService *object,
		const char *contact_path, GError **error);



OphonekitdContactsService *ophonekitd_contacts_service_new(void);



#endif
