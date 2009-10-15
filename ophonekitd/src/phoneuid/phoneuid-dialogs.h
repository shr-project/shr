#ifndef _PHONEUID_DIALOGS_H
#define _PHONEUID_DIALOGS_H

#define PHONEUID_TYPE_DIALOGS_SERVICE            (phoneuid_dialogs_service_get_type ())
#define PHONEUID_DIALOGS_SERVICE(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), PHONEUID_TYPE_DIALOGS_SERVICE, PhoneuidDialogsService))
#define PHONEUID_DIALOGS_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PHONEUID_TYPE_DIALOGS_SERVICE, PhoneuidDialogsServiceClass))
#define PHONEUID_IS_DIALOGS_SERVICE(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), PHONEUID_TYPE_DIALOGS_SERVICE))
#define PHONEUID_IS_DIALOGS_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PHONEUID_TYPE_DIALOGS_SERVICE))
#define PHONEUID_DIALOGS_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), PHONEUID_TYPE_DIALOGS_SERVICE, PhoneuidDialogsServiceClass))


typedef struct _PhoneuidDialogsService PhoneuidDialogsService;
typedef struct _PhoneuidDialogsServiceClass PhoneuidDialogsServiceClass;

GType phoneuid_dialogs_service_get_type(void);

struct _PhoneuidDialogsService {
	GObject parent;
};

struct _PhoneuidDialogsServiceClass {
	GObjectClass parent;
};

gboolean
phoneuid_dialogs_service_display_sim_auth(PhoneuidDialogsService *object,
		GError **error);
gboolean
phoneuid_dialogs_service_hide_sim_auth(PhoneuidDialogsService *object,
		GError **error);
gboolean
phoneuid_dialogs_service_display_ussd(PhoneuidDialogsService *object,
		const int mode, const char *message, GError **error);
gboolean
phoneuid_dialogs_service_display_dialog(PhoneuidDialogsService *object,
		const int dialog, GError **error);


PhoneuidDialogsService *phoneuid_dialogs_service_new(void);



#endif
