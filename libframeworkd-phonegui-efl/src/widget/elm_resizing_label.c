#include <Elementary.h>
#include "elm_resizing_label.h"
#include "elm_config.h"

#define ELM_NEW(t) calloc(1, sizeof(t))

/*
TODO (Waiting on raster for a way to set edje font size from within app): 
- Use real font size changing instead of 2 edje parts.
- Determine canvas width and calculate automaticly when to go to an smaller font size.
- Cleanup more unused parts.
*/

typedef struct _Widget_Data Widget_Data;

struct _Widget_Data
{
    Evas_Object *ent;
    char* entry;
    char* entry2;
    Ecore_Job *deferred_recalc_job;
    Ecore_Event_Handler *sel_notify_handler; 
    Ecore_Event_Handler *sel_clear_handler;
    const char *cut_sel;
    Evas_Coord lastw;
    Evas_Bool changed : 1;
    Evas_Bool linewrap : 1;
    Evas_Bool single_line : 1;
    Evas_Bool password : 1;
    Evas_Bool editable : 1;
    Evas_Bool selection_asked : 1;
    Evas_Bool have_selection : 1;
};

static void _del_hook(Evas_Object *obj);
static void _theme_hook(Evas_Object *obj);
static void _sizing_eval(Evas_Object *obj);
static void _on_focus_hook(void *data, Evas_Object *obj);
static void _resize(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _signal_entry_changed(void *data, Evas_Object *obj, const char *emission, const char *source);

static Eina_List *entries = NULL;

static void
_del_hook(Evas_Object *obj)
{
    Widget_Data *wd = elm_widget_data_get(obj);
    entries = eina_list_remove(entries, obj); 
    if (wd->cut_sel) eina_stringshare_del(wd->cut_sel);
    if (wd->deferred_recalc_job) ecore_job_del(wd->deferred_recalc_job);
    free(wd->entry);
    free(wd);
}

static void
_theme_hook(Evas_Object *obj)
{
    Widget_Data *wd = elm_widget_data_get(obj);
    edje_object_file_set(wd->ent, ELM_RESIZING_LABEL, "elm/resizing-label/base/default");
    _sizing_eval(obj);
}

static void
_elm_win_recalc_job(void *data)
{
    Widget_Data *wd = elm_widget_data_get(data);
    Evas_Coord minw = -1, minh = -1, maxw = -1, maxh = -1;
    Evas_Coord resw, resh, minminw;

    wd->deferred_recalc_job = NULL;
    evas_object_geometry_get(wd->ent, NULL, NULL, &resw, &resh);
    resh = 0;
    minminw = 0;
    edje_object_size_min_restricted_calc(wd->ent, &minw, &minh, 0, 0);
    minminw = minw;
    edje_object_size_min_restricted_calc(wd->ent, &minw, &minh, resw, 0);
    evas_object_size_hint_min_set(data, minminw, minh);
    evas_object_size_hint_max_set(data, minminw, maxh);
}

static void
_sizing_eval(Evas_Object *obj)
{
    Widget_Data *wd = elm_widget_data_get(obj);
    Evas_Coord minw = -1, minh = -1, maxw = -1, maxh = -1;
    Evas_Coord resw, resh, minminw;

    if (wd->linewrap)
    {
        evas_object_geometry_get(wd->ent, NULL, NULL, &resw, &resh);
        if ((resw == wd->lastw) && (!wd->changed)) return;
            wd->changed = 0;
        wd->lastw = resw;
        if (wd->deferred_recalc_job) ecore_job_del(wd->deferred_recalc_job);
            wd->deferred_recalc_job = ecore_job_add(_elm_win_recalc_job, obj);
    }
    else
    {
        evas_object_geometry_get(wd->ent, NULL, NULL, &resw, &resh);
        edje_object_size_min_calc(wd->ent, &minw, &minh);
        evas_object_size_hint_min_set(obj, minw, minh);
        evas_object_size_hint_max_set(obj, maxw, maxh);
    }
}

static void
_resize(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Widget_Data *wd = elm_widget_data_get(data);
   if (wd->linewrap) _sizing_eval(data);
   Evas_Coord ww, hh;
   evas_object_geometry_get(wd->ent, NULL, NULL, &ww, &hh);
}

static void
_signal_entry_changed(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Widget_Data *wd = elm_widget_data_get(data);
   evas_object_smart_callback_call(data, "changed", NULL);
   wd->changed = 1;
   _sizing_eval(data);
}

EAPI Evas_Object *
elm_resizing_label_add(Evas_Object *parent)
{
   Evas_Object *obj;
   Evas *e;
   Widget_Data *wd;
   
   wd = ELM_NEW(Widget_Data);
   e = evas_object_evas_get(parent);
   obj = elm_widget_add(e);
   elm_widget_on_focus_hook_set(obj, _on_focus_hook, NULL);
   elm_widget_data_set(obj, wd);
   elm_widget_del_hook_set(obj, _del_hook);
   elm_widget_theme_hook_set(obj, _theme_hook);
   elm_widget_can_focus_set(obj, 1);

   wd->linewrap = 1;
   wd->editable = 0;
   wd->entry = "";
   
   wd->ent = edje_object_add(e);
   evas_object_event_callback_add(wd->ent, EVAS_CALLBACK_RESIZE, _resize, obj);
					  
   _theme_hook(obj);
   edje_object_signal_callback_add(wd->ent, "entry,changed", "elm.text.big", _signal_entry_changed, obj);
   edje_object_part_text_set(wd->ent, "elm.text.big", "<br>");
   edje_object_part_text_set(wd->ent, "elm.text.small", "<br>");
   elm_widget_resize_object_set(obj, wd->ent);
   _sizing_eval(obj);
   
   entries = eina_list_prepend(entries, obj);
   return obj;
}

EAPI void
elm_resizing_label_label_set(Evas_Object *obj, const char *entry)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   int i;
    
   if(!wd->entry) free(wd->entry); 
       
   wd->entry = malloc( 68*sizeof(char)); 
   strcpy(wd->entry, entry );
   int length = strlen(wd->entry); 

   if (!wd->entry) wd->entry = "<br>";
    
   if( length < 9  )
       edje_object_signal_emit(wd->ent, "big", "elm");
   else
       edje_object_signal_emit(wd->ent, "small", "elm");
   
   // adding the ... in front if there are to much numbers 
   if( length > 26  ) {
        char* tmp = strdup(wd->entry);
        strcpy( wd->entry, "...");
        strcpy( wd->entry+3, wd->entry + length - 23 );
   }

   // putting everything on 2 lines.
   if( length > 13  ) {
        char* tmp = malloc( 64*sizeof(char) );
        strcpy(tmp, wd->entry+13);
        wd->entry[13] = '\0';
        strcat(wd->entry, "<br>");
        strcat(wd->entry, tmp);
        
        free(tmp);
   }

   edje_object_part_text_set(wd->ent, "elm.text.big", wd->entry);
   edje_object_part_text_set(wd->ent, "elm.text.small", wd->entry);
   wd->changed = 1;
   _sizing_eval(obj);
}

EAPI const char *
elm_resizing_label_label_get(Evas_Object *obj)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   return edje_object_part_text_get(wd->ent, "elm.text.big");
}
