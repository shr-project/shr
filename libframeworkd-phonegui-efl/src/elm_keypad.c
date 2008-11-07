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
   Evas_Object *widget, *keypad, *bt1, *bt2, *bt3, *bt4, *bt5, *bt6, *bt7, *bt8, *bt9, *bt0, *bt_star, *bt_rhomb;
};

static void _del_hook(Evas_Object *obj);
static void _sizing_eval(Evas_Object *obj);
static void _changed_size_hints(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _sub_del(void *data, Evas_Object *obj, void *event_info);
static void _signal_clicked(void *data, Evas_Object *obj, void *event_info);

static void
_del_hook(Evas_Object *obj)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   evas_object_del(wd->bt0);
   evas_object_del(wd->bt1);
   evas_object_del(wd->bt2);
   evas_object_del(wd->bt3);
   evas_object_del(wd->bt4);
   evas_object_del(wd->bt5);
   evas_object_del(wd->bt6);
   evas_object_del(wd->bt7);
   evas_object_del(wd->bt8);
   evas_object_del(wd->bt9);
   evas_object_del(wd->bt_star);
   evas_object_del(wd->bt_rhomb);
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
_signal_clicked(void *data, Evas_Object *obj, void *event_info)
{
   g_debug("BUTTON CLICKED!");
   Widget_Data *wd = elm_widget_data_get(data);

   char input;
   if(obj == wd->bt0)
      input = '0';
   else if(obj == wd->bt1)
      input = '1';
   else if(obj == wd->bt2)
      input = '2';
   else if(obj == wd->bt3)
      input = '3';
   else if(obj == wd->bt4)
      input = '4';
   else if(obj == wd->bt5)
      input = '5';
   else if(obj == wd->bt6)
      input = '6';
   else if(obj == wd->bt7)
      input = '7';
   else if(obj == wd->bt8)
      input = '8';
   else if(obj == wd->bt9)
      input = '9';
   else if(obj == wd->bt_star)
      input = '*';
   else if(obj == wd->bt_rhomb)
      input = '#';
   else
      return;

   g_debug("Input: %c", input);
   evas_object_smart_callback_call(wd->widget, "clicked", input);
   g_debug("after");
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


   /*
    * Swallow buttons
    */
   wd->bt1 = elm_button_add(wd->keypad);
   elm_button_label_set(wd->bt1, "1");
   evas_object_smart_callback_add(wd->bt1, "clicked", _signal_clicked, wd->widget);
   edje_object_part_swallow(wd->keypad, "button1", wd->bt1);
   evas_object_show(wd->bt1);

   wd->bt2 = elm_button_add(wd->keypad);
   elm_button_label_set(wd->bt2, "2");
   evas_object_smart_callback_add(wd->bt2, "clicked", _signal_clicked, wd->widget);
   edje_object_part_swallow(wd->keypad, "button2", wd->bt2);
   evas_object_show(wd->bt2);

   wd->bt3 = elm_button_add(wd->keypad);
   elm_button_label_set(wd->bt3, "3");
   evas_object_smart_callback_add(wd->bt3, "clicked", _signal_clicked, wd->widget);
   edje_object_part_swallow(wd->keypad, "button3", wd->bt3);
   evas_object_show(wd->bt3);

   wd->bt4 = elm_button_add(wd->keypad);
   elm_button_label_set(wd->bt4, "4");
   evas_object_smart_callback_add(wd->bt4, "clicked", _signal_clicked, wd->widget);
   edje_object_part_swallow(wd->keypad, "button4", wd->bt4);
   evas_object_show(wd->bt4);

   wd->bt5 = elm_button_add(wd->keypad);
   elm_button_label_set(wd->bt5, "5");
   evas_object_smart_callback_add(wd->bt5, "clicked", _signal_clicked, wd->widget);
   edje_object_part_swallow(wd->keypad, "button5", wd->bt5);
   evas_object_show(wd->bt5);

   wd->bt6 = elm_button_add(wd->keypad);
   elm_button_label_set(wd->bt6, "6");
   evas_object_smart_callback_add(wd->bt6, "clicked", _signal_clicked, wd->widget);
   edje_object_part_swallow(wd->keypad, "button6", wd->bt6);
   evas_object_show(wd->bt6);

   wd->bt7 = elm_button_add(wd->keypad);
   elm_button_label_set(wd->bt7, "7");
   evas_object_smart_callback_add(wd->bt7, "clicked", _signal_clicked, wd->widget);
   edje_object_part_swallow(wd->keypad, "button7", wd->bt7);
   evas_object_show(wd->bt7);

   wd->bt8 = elm_button_add(wd->keypad);
   elm_button_label_set(wd->bt8, "8");
   evas_object_smart_callback_add(wd->bt8, "clicked", _signal_clicked, wd->widget);
   edje_object_part_swallow(wd->keypad, "button8", wd->bt8);
   evas_object_show(wd->bt8);

   wd->bt9 = elm_button_add(wd->keypad);
   elm_button_label_set(wd->bt9, "9");
   evas_object_smart_callback_add(wd->bt9, "clicked", _signal_clicked, wd->widget);
   edje_object_part_swallow(wd->keypad, "button9", wd->bt9);
   evas_object_show(wd->bt9);

   wd->bt0 = elm_button_add(wd->keypad);
   elm_button_label_set(wd->bt0, "0");
   evas_object_smart_callback_add(wd->bt0, "clicked", _signal_clicked, wd->widget);
   edje_object_part_swallow(wd->keypad, "button0", wd->bt0);
   evas_object_show(wd->bt0);

   wd->bt_star = elm_button_add(wd->keypad);
   elm_button_label_set(wd->bt_star, "*");
   evas_object_smart_callback_add(wd->bt_star, "clicked", _signal_clicked, wd->widget);
   edje_object_part_swallow(wd->keypad, "button_star", wd->bt_star);
   evas_object_show(wd->bt_star);

   wd->bt_rhomb = elm_button_add(wd->keypad);
   elm_button_label_set(wd->bt_rhomb, "#");
   evas_object_smart_callback_add(wd->bt_rhomb, "clicked", _signal_clicked, wd->widget);
   edje_object_part_swallow(wd->keypad, "button_rhomb", wd->bt_rhomb);
   evas_object_show(wd->bt_rhomb);


   elm_widget_resize_object_set(wd->widget, wd->keypad);
   //evas_object_smart_callback_add(wd->widget, "sub-object-del", _sub_del, wd->widget);
   
   _sizing_eval(wd->widget);
   return wd->widget;
}

