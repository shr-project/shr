#ifndef _ELM_KEYPAD_H
#define _ELM_KEYPAD_H

#include <Elementary.h>

Evas_Object* elm_resizing_label_add(Evas_Object *parent);
void elm_resizing_label_label_set(Evas_Object *obj, const char *entry);
const char* elm_resizing_label_label_get(Evas_Object *obj);

#endif
