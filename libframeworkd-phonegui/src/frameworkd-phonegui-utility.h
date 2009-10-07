#ifndef _FRAMEWORKD_PHONEGUI_UTILITY_H
#define _FRAMEWORKD_PHONEGUI_UTILITY_H
#include <glib.h>

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


void phonegui_contact_lookup(const char *number,
			     void (*name_callback) (GError *, char *, gpointer),
			     void *data);
int phonegui_sms_send(const char *message, GPtrArray * recipients,
		void *callback1, void *callback2);
int phonegui_call_initiate(const char *number,
				void (*callback)(GError *, int id_call, gpointer),
				gpointer userdata);
int phonegi_call_release(int call_id, 
			void (*callback)(GError *, int id_call, gpointer),
			gpointer userdata)
#endif
