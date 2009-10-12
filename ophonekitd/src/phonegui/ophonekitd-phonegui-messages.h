#ifndef _OPHONEKITD_PHONEGUI_MESSAGES_H
#define _OPHONEKITD_PHONEGUI_MESSAGES_H

#define OPHONEKITD_TYPE_MESSAGES_SERVICE            (ophonekitd_messages_service_get_type ())
#define OPHONEKITD_MESSAGES_SERVICE(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), OPHONEKITD_TYPE_MESSAGES_SERVICE, OphonekitdMessagesService))
#define OPHONEKITD_MESSAGES_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), OPHONEKITD_TYPE_MESSAGES_SERVICE, OphonekitdMessagesServiceClass))
#define OPHONEKITD_IS_MESSAGES_SERVICE(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), OPHONEKITD_TYPE_MESSAGES_SERVICE))
#define OPHONEKITD_IS_MESSAGES_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), OPHONEKITD_TYPE_MESSAGES_SERVICE))
#define OPHONEKITD_MESSAGES_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), OPHONEKITD_TYPE_MESSAGES_SERVICE, OphonekitdMessagesServiceClass))


typedef struct _OphonekitdMessagesService OphonekitdMessagesService;
typedef struct _OphonekitdMessagesServiceClass OphonekitdMessagesServiceClass;

GType ophonekitd_messages_service_get_type(void);

struct _OphonekitdMessagesService {
	GObject parent;
};

struct _OphonekitdMessagesServiceClass {
	GObjectClass parent;
};

gboolean
ophonekitd_messages_service_display_list(OphonekitdMessagesService *object,
		GError **error);
gboolean
ophonekitd_messages_service_display_item(OphonekitdMessagesService *object,
		const char *message_path, GError **error);
gboolean
ophonekitd_messages_service_create_message(OphonekitdMessagesService *object,
		GHashTable *values, GError **error);


OphonekitdMessagesService *ophonekitd_messages_service_new(void);



#endif

