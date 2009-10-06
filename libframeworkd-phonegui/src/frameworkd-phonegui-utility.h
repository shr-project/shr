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
char *phonegui_contact_cache_lookup(char *number);
void phonegui_contact_lookup(const char *number,
			     void (*name_callback) (GError *, char *, gpointer),
			     void *data);
void phonegui_init_contacts_cache();
void phonegui_destroy_contacts_cache();
phonegui_send_sms(const char *message, GPtrArray * recipients, void *callback1,
		  void *callback2);

#endif
