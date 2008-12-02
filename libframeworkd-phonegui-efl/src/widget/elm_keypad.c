#include "elm_keypad.h"

// for g_debug()
#include <glib-2.0/glib.h>

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/keypad.edj"

// TODO: Talk about it with raster
// got it from elm_priv.h
#define ELM_NEW(t) calloc(1, sizeof(t))

typedef struct _Widget_Data Widget_Data;

struct _Widget_Data
{
   Evas_Object *widget, *keypad;
};

static void _del_hook(Evas_Object *obj);
static void _sizing_eval(Evas_Object *obj);
static void _changed_size_hints(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _sub_del(void *data, Evas_Object *obj, void *event_info);
static void _signal_clicked(void *data, Evas_Object *o, const char *emission, const char *source);

static void
_del_hook(Evas_Object *obj)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   evas_object_del(wd->keypad);
   free(wd);
}

static void
_sizing_eval(Evas_Object *obj)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   Evas_Coord minw = -1, minh = -1, maxw = -1, maxh = -1;
 
   edje_object_size_min_calc(wd->keypad, &minw, &minh);
   evas_object_size_hint_min_set(obj, minw, minh);
   evas_object_size_hint_max_set(obj, maxw, maxh);
}

static void
_changed_size_hints(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   return; // TODO: Confirm this line

   //Widget_Data *wd = elm_widget_data_get(data);
   //if (obj != wd->icon) return;
   //edje_object_part_swallow(wd->widget, "elm.swallow.content", obj);
   //_sizing_eval(data);
}

/*
static void
_sub_del(void *data, Evas_Object *obj, void *event_info)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   Evas_Object *sub = event_info;
   if (sub == wd->icon)
     {
	edje_object_signal_emit(wd->keypad, "elm,state,icon,hidden", "elm");
	evas_object_event_callback_del
	  (sub, EVAS_CALLBACK_CHANGED_SIZE_HINTS, _changed_size_hints);
	wd->icon = NULL;
	_sizing_eval(obj);
     }
   
}
*/

static void
_signal_clicked(void *data, Evas_Object *o, const char *emission, const char *source)
{
   Widget_Data *wd = elm_widget_data_get(data);
   evas_object_smart_callback_call(wd->widget, "clicked", emission[0]);
}

EAPI Evas_Object *
elm_keypad_add(Evas_Object *parent)
{
   // Evas_Object *obj; Instead I'm using the wd->widget variable
   Evas *e;
   Widget_Data *wd;
   
   wd = ELM_NEW(Widget_Data);
   e = evas_object_evas_get(parent);
   wd->widget = elm_widget_add(e);
   elm_widget_data_set(wd->widget, wd);
   elm_widget_del_hook_set(wd->widget, _del_hook);
   
   wd->keypad = edje_object_add(e);
   g_debug("keypad evas object: %d", wd->keypad);
   edje_object_file_set(wd->keypad, UI_FILE, "keypad");
   edje_object_signal_callback_add(wd->keypad, "*", "input", _signal_clicked, wd->widget);


   elm_widget_resize_object_set(wd->widget, wd->keypad);
   //evas_object_smart_callback_add(wd->widget, "sub-object-del", _sub_del, wd->widget);
   
   _sizing_eval(wd->widget);
   return wd->widget;
}

