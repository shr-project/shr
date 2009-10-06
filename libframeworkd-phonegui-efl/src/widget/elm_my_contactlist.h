#ifndef _ELM_MY_CONTACTLIST_H
#define _ELM_MY_CONTACTLIST_H

#include <Elementary.h>
#include <glib.h>

Evas_Object *elm_my_contactlist_add(Evas_Object * parent);
GHashTable *elm_my_contactlist_selected_row_get(void *data);
void elm_my_contactlist_refresh(void *data);

#endif
