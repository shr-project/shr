#ifndef _ELM_KEYPAD_H
#define _ELM_KEYPAD_H

#include <Elementary.h>

EAPI Evas_Object* elm_resizing_label_add(Evas_Object *parent);
EAPI void elm_resizing_label_label_set(Evas_Object *obj, const char *entry);
EAPI const char* elm_resizing_label_label_get(Evas_Object *obj);

#endif
