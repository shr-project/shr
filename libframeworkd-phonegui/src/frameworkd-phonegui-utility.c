#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <phone-utils.h>
#include <assert.h>
#include <time.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>

#include "frameworkd-phonegui-utility.h"

extern Settings *conf;

static GValue *
_new_gvalue_string(const char *value)
{
	GValue *val = calloc(1, sizeof(GValue));
	if (!val) {
		return NULL;
	}
	g_value_init(val, G_TYPE_STRING);
	g_value_set_string(val, value);

	return val;
}

static GValue *
_new_gvalue_int(int value)
{
	GValue *val = calloc(1, sizeof(GValue));
	if (!val) {
		return NULL;
	}
	g_value_init(val, G_TYPE_INT);
	g_value_set_int(val, value);

	return val;
}

static GValue *
_new_gvalue_boolean(int value)
{
	GValue *val = calloc(1, sizeof(GValue));
	if (!val) {
		return NULL;
	}
	g_value_init(val, G_TYPE_BOOLEAN);
	g_value_set_boolean(val, value);

	return val;
}

static
  guint
phone_number_hash(gconstpointer v)
{
	gchar *n = phone_utils_normalize_number((char *) v);
	guint ret = g_str_hash(n);
	g_free(n);
	return (ret);
}

void
cache_phonebook_entry(GValueArray * entry, void *data)
{
	char *number =
		strdup(g_value_get_string(g_value_array_get_nth(entry, 2)));
	char *name =
		strdup(g_value_get_string(g_value_array_get_nth(entry, 1)));
	g_hash_table_insert(conf->contact_cache, number, name);
}


void
cache_phonebook_callback(GError * error, GPtrArray * contacts,
			 gpointer userdata)
{
	g_debug("cache_phonebook_callback called");
	if (error == NULL && contacts != NULL) {
		g_debug("creating contact_cache");
		conf->contact_cache =
			g_hash_table_new_full(phone_number_hash,
					      phone_utils_numbers_equal, free,
					      free);
		if (!conf->contact_cache) {
			g_warning("could not allocate contact cache");
			return;
		}
		g_ptr_array_foreach(contacts, cache_phonebook_entry, NULL);
	}
	else
		g_debug("caching phonebook failed: %s %s %d", error->message,
			g_quark_to_string(error->domain), error->code);

}

static char *
_lookup_add_prefix(const char *_number)
{
	char *number = NULL;
	if (strncmp(_number, "tel:", 4)) {
		number = malloc(strlen(_number) + 5);	/* 5 is for "tel:" and the null */
		if (!number) {
			return NULL;
		}
		strcpy(number, "tel:");
		strcat(number, _number);
	}


	return number;
}

int
phonegui_contact_lookup(const char *_number,
			void (*name_callback) (GError *, char *, gpointer),
			void *data)
{
	GHashTable *query =
		g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
	char *number = _lookup_add_prefix(_number);
	if (!number) {
		return 1;
	}

	g_debug("Attempting to resolve name for: \"%s\"", number);


	GValue *value = _new_gvalue_string((number) ? number : _number);	/*  we prefer using number */
	if (!value) {
		free(number);
		return 1;
	}
	g_hash_table_insert(query, "Phone", value);


	opimd_contacts_get_single_entry_single_field(query, "Name",
						     name_callback, data);

	if (number)
		free(number);

	return 0;
}

char *
phonegui_contact_cache_lookup(char *number)
{
	if (conf->contact_cache == NULL)
		return (number);
	g_debug("looking for '%s' in contacts_cache", number);
	if (!number || !*number || !strcmp(number, "*****")) {
		g_debug("contact_cache: got unknown number");
		return ("");
	}
	if (*number == '"') {
		number++;
		char *s = number;
		while (*s) {
			if (*s == '"') {
				*s = '\0';
				break;
			}
			s++;
		}
	}

	char *name = g_hash_table_lookup(conf->contact_cache, number);
	if (name && *name) {
		g_debug("found name '%s'", name);
		return (name);
	}
	return (number);
}

void
phonegui_init_contacts_cache()
{
	ogsmd_sim_retrieve_phonebook("contacts", cache_phonebook_callback,
				     NULL);
}

void
phonegui_destroy_contacts_cache()
{
	g_hash_table_destroy(conf->contact_cache);
}


static void
_add_opimd_message(const char *number, const char *message)
{
	/*FIXME: ATM it just saves it as saved and tell the user everything
	 * is ok, even if it didn't save. We really need to fix that,
	 * we should verify if frameworkd-glib's callbacks work */
	/*TODO: add timzone and handle messagesent correctly */
	/* add to opimd */

	GHashTable *message_opimd =
		g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free);
	GValue *tmp;

	tmp = _new_gvalue_string(number);
	g_hash_table_insert(message_opimd, "Recipient", tmp);

	tmp = _new_gvalue_string("out");
	g_hash_table_insert(message_opimd, "Direction", tmp);

	tmp = _new_gvalue_string("SMS");
	g_hash_table_insert(message_opimd, "Folder", tmp);

	tmp = _new_gvalue_string(message);
	g_hash_table_insert(message_opimd, "Content", tmp);

	tmp = _new_gvalue_boolean(1);
	g_hash_table_insert(message_opimd, "MessageSent", tmp);

	tmp = _new_gvalue_int(time(NULL));
	g_hash_table_insert(message_opimd, "Timestamp", tmp);

	opimd_messages_add(message_opimd, NULL, NULL);

	g_hash_table_destroy(message_opimd);
}

int
phonegui_sms_send(const char *message, GPtrArray * recipients, void *callback1,
		  void *callback2)
/* FIXME: add real callbacks types when I find out */
{
	/*FIXME: seems ok, though should verify for potential memory leaks */
	int len;
	int ucs;
	int i;
	int csm_num = 0;
	int csm_id = 1;		/* do we need a better way? */
	int csm_seq;
	int ret_val = 0;
	char **messages;
	GHashTable *options =
		g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free);
	GValue *alphabet, *val_csm_num, *val_csm_id, *val_csm_seq;
	alphabet = val_csm_num = val_csm_id = val_csm_seq = NULL;
	if (!options) {
		ret_val = 1;
		goto end;
	}
	if (!recipients) {
		ret_val = 1;
		goto clean_hash;
	}

	len = phone_utils_gsm_sms_strlen(message);
	ucs = phone_utils_gsm_is_ucs(message);


	/* set alphabet if needed */
	if (ucs) {
		g_debug("Sending message as ucs2");
		alphabet = _new_gvalue_string("ucs2");
		if (!alphabet) {
			ret_val = 1;
			goto clean_hash;
		}
		g_hash_table_insert(options, "alphabet", alphabet);
	}
	else {
		g_debug("Sending message as gsm default");
	}

	messages = phone_utils_gsm_sms_split_message(message, len, ucs);
	if (!messages) {
		ret_val = 1;
		goto clean_gvalues;
	}
	/* conut the number of messages */
	for (csm_num = 0; messages[csm_num]; csm_num++);

	if (csm_num > 1) {
		val_csm_num = _new_gvalue_int(csm_num);
		if (!val_csm_num) {
			ret_val = 1;
			goto clean_gvalues;
		}
		val_csm_id = _new_gvalue_int(csm_id);
		if (!val_csm_id) {
			ret_val = 1;
			goto clean_gvalues;
		}

		g_hash_table_insert(options, "csm_id", val_csm_id);
		g_hash_table_insert(options, "csm_num", val_csm_num);
	}
	else if (csm_num == 0) {	/* nothing to do */
		ret_val = 1;
		goto clean_messages;
	}

	g_debug("Sending %d parts with total length of %d", csm_num, len);

	/* cycle through all the recipients */
	for (i = 0; i < recipients->len; i++) {
		GHashTable *properties =
			(GHashTable *) g_ptr_array_index(recipients, i);
		char *number =
			(char *) g_hash_table_lookup(properties, "number");
		assert(number != NULL);

		if (csm_num > 1) {
			for (csm_seq = 1; csm_seq <= csm_num; csm_seq++) {
				val_csm_seq = _new_gvalue_int(csm_seq);
				if (!val_csm_seq) {
					ret_val = 1;
					goto clean_messages;
				}

				g_debug("sending sms number %d", csm_seq);
				g_hash_table_replace(options, "csm_seq",
						     val_csm_seq);
				ogsmd_sms_send_message(number,
						       messages[csm_seq - 1],
						       options, NULL, NULL);

			}
		}
		else {
			ogsmd_sms_send_message(number, messages[0], options,
					       NULL, NULL);
		}
		_add_opimd_message(number, message);
	}

      clean_messages:
	/* clean up */
	for (i = 0; messages[i]; i++) {
		free(messages[i]);
	}
	free(messages);

      clean_gvalues:
#if 0
	/* FIXME: being done when hash table clears itself,
	 * should probably do it on my own.
	 * this will save me the reallocation in the loop (before the
	 * actual send*/
	if (alphabet)
		free(alphabet);
	if (val_csm_num)
		free(val_csm_num);
	if (val_csm_id)
		free(val_csm_id);
	if (val_csm_seq)
		free(val_csm_seq);
#endif
      clean_hash:

	g_hash_table_destroy(options);

      end:
	return 0;

}


int
phonegui_call_initiate(const char *number,
			void (*callback)(GError *, int id_call, gpointer),
			gpointer userdata)
{
	g_debug("Inititating a call to %s\n", number);
	ogsmd_call_initiate(number, "voice", callback, userdata);
	return 0;
}

int
phonegui_call_release(int call_id, 
			void (*callback)(GError *, int id_call, gpointer),
			gpointer userdata)
{
	ogsmd_call_release(call_id, callback, userdata);
	return 0;
}

int
phonegui_call_activate(int call_id, 
			void (*callback)(GError *, int id_call, gpointer),
			gpointer userdata)
{
	ogsmd_call_activate(call_id, callback, userdata);
	return 0;
}

int
phonegui_contact_delete(const char *path,
				void (*callback) (GError *, char *, gpointer),
				void *data)
{
	opimd_contact_delete(path, callback, data);
	return 0;
}
