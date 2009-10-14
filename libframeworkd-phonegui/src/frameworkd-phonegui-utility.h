#ifndef _FRAMEWORKD_PHONEGUI_UTILITY_H
#define _FRAMEWORKD_PHONEGUI_UTILITY_H
#include <glib.h>

typedef enum {
	PHONEGUI_DIALOG_ERROR_DO_NOT_USE,
	// This value is used for checking if we get a wrong pointer out of a HashTable. 
	// So do not use it, and leave it first in this enum. ( because 0 == NULL )
	PHONEGUI_DIALOG_MESSAGE_STORAGE_FULL,
	PHONEGUI_DIALOG_SIM_NOT_PRESENT
} PhoneguiDialogType;

typedef struct {
	gchar *library;
	GHashTable *contact_cache;
} Settings;

gchar *phonegui_get_user_home_prefix();
gchar *phonegui_get_user_home_code();
/* soon to be deleted functions */
char *phonegui_contact_cache_lookup(char *number);
void phonegui_init_contacts_cache();
void phonegui_destroy_contacts_cache();
/* end of soon to be deleted */


int phonegui_contact_lookup(const char *number,
			     void (*name_callback) (GError *, char *, gpointer),
			     void *data);
int phonegui_contact_delete(const char *path,
				void (*name_callback) (GError *, char *, gpointer),
				void *data);
/* FIXME: rename to message send */
int phonegui_sms_send(const char *message, GPtrArray * recipients,
		void *callback1, void *callback2);
int phonegui_message_delete(const char *message_path,
				void (*callback)(GError *, gpointer),
				void *data);

int phonegui_call_initiate(const char *number,
				void (*callback)(GError *, int id_call, gpointer),
				gpointer userdata);
int phonegui_call_release(int call_id, 
			void (*callback)(GError *, int id_call, gpointer),
			gpointer userdata);
int phonegui_call_activate(int call_id, 
			void (*callback)(GError *, int id_call, gpointer),
			gpointer userdata);
int phonegui_call_send_dtmf(const char *tones,
				void (*callback)(GError *, gpointer),
				gpointer userdata);

int phonegui_network_send_ussd_request(char *request,
				void (*callback)(GError *, gpointer),
				gpointer userdata);

#endif
