#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <phone-utils.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-dbus.h>
#include <frameworkd-glib/ogsmd/frameworkd-glib-ogsmd-sim.h>

#include "frameworkd-phonegui-utility.h"

extern Settings *conf;
static
guint phone_number_hash(gconstpointer v)
{
    gchar *n = phone_utils_normalize_number((char *)v);
    guint ret = g_str_hash(n);
    g_free(n);
    return (ret);
}

void cache_phonebook_entry(GValueArray *entry, void *data) {
    char *number = strdup(g_value_get_string(g_value_array_get_nth(entry, 2)));
    char *name = strdup(g_value_get_string(g_value_array_get_nth(entry, 1)));
    g_hash_table_insert(conf->contact_cache, number, name);
}


void cache_phonebook_callback(GError *error, GPtrArray *contacts, gpointer userdata) {
    g_debug("cache_phonebook_callback called");
    if(error == NULL && contacts != NULL) {
        g_debug("creating contact_cache");
        conf->contact_cache = g_hash_table_new_full(phone_number_hash, phone_utils_numbers_equal, free, free);
        if (!conf->contact_cache) {
            g_warning("could not allocate contact cache");
            return;
        }
        g_ptr_array_foreach(contacts, cache_phonebook_entry, NULL);
    }
    else
        g_debug("caching phonebook failed: %s %s %d", error->message, g_quark_to_string(error->domain), error->code);

}

static char *
_lookup_add_prefix(const char *_number)
{
	char *number = NULL;
	if (strncmp(_number, "tel:", 4)) {
		number = malloc(strlen(_number) + 5); /* 5 is for "tel:" and the null */
		if (!number) {
			return NULL;
		}
		strcpy(number, "tel:");
		strcat(number, _number);
	}


	return number;
}

void
phonegui_contact_lookup(const char *_number, void (*name_callback)(GError *, char *, gpointer), void *data)
{
	GHashTable *query = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
	char *number = _lookup_add_prefix(_number);
		
	g_debug("number = %s --> querying name...", _number);

	
	GValue *value = g_slice_alloc0(sizeof(GValue));
	g_value_init(value, G_TYPE_STRING);
	g_value_set_string(value, (number) ? number : _number); /*  we prefer using number */
	g_hash_table_insert(query, "Phone", value);


	opimd_contacts_get_single_entry_single_field(query, "Name", name_callback, data);

	if (number)
		free(number);

}

char *phonegui_contact_cache_lookup(char *number) {
    if(conf->contact_cache == NULL) return (number);
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

void phonegui_init_contacts_cache() {
    ogsmd_sim_retrieve_phonebook("contacts", cache_phonebook_callback, NULL);
}

void phonegui_destroy_contacts_cache() {
    g_hash_table_destroy(conf->contact_cache);
}