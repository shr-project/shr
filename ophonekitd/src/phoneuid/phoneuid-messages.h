#ifndef _PHONEUID_MESSAGES_H
#define _PHONEUID_MESSAGES_H

#define PHONEUID_TYPE_MESSAGES_SERVICE            (phoneuid_messages_service_get_type ())
#define PHONEUID_MESSAGES_SERVICE(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), PHONEUID_TYPE_MESSAGES_SERVICE, PhoneuidMessagesService))
#define PHONEUID_MESSAGES_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PHONEUID_TYPE_MESSAGES_SERVICE, PhoneuidMessagesServiceClass))
#define PHONEUID_IS_MESSAGES_SERVICE(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), PHONEUID_TYPE_MESSAGES_SERVICE))
#define PHONEUID_IS_MESSAGES_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PHONEUID_TYPE_MESSAGES_SERVICE))
#define PHONEUID_MESSAGES_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), PHONEUID_TYPE_MESSAGES_SERVICE, PhoneuidMessagesServiceClass))


typedef struct _PhoneuidMessagesService PhoneuidMessagesService;
typedef struct _PhoneuidMessagesServiceClass PhoneuidMessagesServiceClass;

GType phoneuid_messages_service_get_type(void);

struct _PhoneuidMessagesService {
	GObject parent;
};

struct _PhoneuidMessagesServiceClass {
	GObjectClass parent;
};

gboolean
phoneuid_messages_service_display_list(PhoneuidMessagesService *object,
		GHashTable *options, DBusGMethodInvocation *context);
gboolean
phoneuid_messages_service_display_item(PhoneuidMessagesService *object,
		const char *message_path, GError **error);
gboolean
phoneuid_messages_service_create_message(PhoneuidMessagesService *object,
		GHashTable *values, GError **error);


PhoneuidMessagesService *phoneuid_messages_service_new(void);



#endif

