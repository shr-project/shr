#include "elm_my_contactlist.h"
#include <glib.h>
#include <glib-object.h>
#include <Etk.h>
#include "async.h"

#define UI_FILE "/usr/share/libframeworkd-phonegui-efl/contacts.edj"

// TODO: Talk about it with raster
// got it from elm_priv.h
#define ELM_NEW(t) calloc(1, sizeof(t))

typedef struct _Widget_Data Widget_Data;

struct _Widget_Data
{
   Evas_Object *widget, *keypad;
   Etk_Widget *container, *tree;
   Etk_Tree_Col *col1;
   GPtrArray *messages;
   Evas_Object **widget_pointer;
};

static void _del_hook(Evas_Object *obj);
static void _sizing_eval(Evas_Object *obj);
static void _changed_size_hints(void *data, Evas *e, Evas_Object *obj, void *event_info);
//static void _sub_del(void *data, Evas_Object *obj, void *event_info);
static void _signal_clicked(void *data, Evas_Object *o, const char *emission, const char *source);
static void _retrieve_callback2(void *data);
static void _process_entry(GValueArray *entry, void *data);

static void
_del_hook(Evas_Object *obj)
{
   Widget_Data *wd = elm_widget_data_get(obj);
   *(wd->widget_pointer) = NULL;
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
    g_debug("SUB DEL CALLED!");
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


static gint
_compare_entries(GValueArray **a, GValueArray **b) {
    return strcasecmp(
        g_value_get_string(g_value_array_get_nth(*a, 1)),
        g_value_get_string(g_value_array_get_nth(*b, 1))
    );
}


static void
_process_entry(GValueArray *entry, void *data) {
    Widget_Data *wd = elm_widget_data_get(data);

    GHashTable *parameters = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(parameters, strdup("id"), g_value_get_int(g_value_array_get_nth(entry, 0)));
    g_hash_table_insert(parameters, strdup("name"), strdup(g_value_get_string(g_value_array_get_nth(entry, 1))));
    g_hash_table_insert(parameters, strdup("number"), strdup(g_value_get_string(g_value_array_get_nth(entry, 2))));
    Etk_Tree_Row *row = etk_tree_row_append(ETK_TREE(wd->tree), NULL, wd->col1, parameters, NULL);

    etk_tree_row_data_set(row, parameters);
}


static void
_retrieve_callback(GError*error, GPtrArray*messages, void *data) {
    Evas_Object **widget = (Evas_Object **) data;

    if(*widget == NULL) {
        free(widget);
    } else {
        Widget_Data *wd = elm_widget_data_get(*widget);
        wd->messages = messages;
        async_trigger(_retrieve_callback2, data);
    }
}

static void
_retrieve_callback2(void *data) {
    Evas_Object **widget = (Evas_Object **) data;

    if(*widget == NULL) {
        free(widget);
    } else {
        Widget_Data *wd = elm_widget_data_get(*widget);
        GPtrArray *messages = wd->messages;

        g_ptr_array_sort(messages, _compare_entries);
        g_ptr_array_foreach(messages, _process_entry, *widget);
    }
}


EAPI Evas_Object *
elm_my_contactlist_add(Evas_Object *parent)
{
   // Evas_Object *obj; Instead I'm using the wd->widget variable
   Evas *e;
   Widget_Data *wd;
   
   wd = ELM_NEW(Widget_Data);
   e = evas_object_evas_get(parent);
   wd->widget = elm_widget_add(e);
   elm_widget_data_set(wd->widget, wd);

   wd->widget_pointer = malloc(sizeof(Evas_Object **));
   *(wd->widget_pointer) = wd->widget;
   elm_widget_del_hook_set(wd->widget, _del_hook);
   
  /* wd->keypad = edje_object_add(e);
   g_debug("keypad evas object: %d", wd->keypad);
   edje_object_file_set(wd->keypad, UI_FILE, "keypad");
   edje_object_signal_callback_add(wd->keypad, "*", "input", _signal_clicked, wd->widget);*/


    wd->tree = etk_tree_new();
    etk_tree_rows_height_set(ETK_TREE(wd->tree), 80);
    etk_tree_mode_set(ETK_TREE(wd->tree), ETK_TREE_MODE_LIST);
    etk_tree_headers_visible_set(ETK_TREE(wd->tree), ETK_FALSE);
    etk_tree_multiple_select_set(ETK_TREE(wd->tree), ETK_FALSE);

    wd->col1 = etk_tree_col_new(ETK_TREE(wd->tree), "Title", 300, 0.0);
    etk_tree_col_model_add(wd->col1, etk_tree_model_edje_new(UI_FILE, "row"));
    etk_tree_build(ETK_TREE(wd->tree));

    Etk_Scrolled_View *scrolled_view = etk_tree_scrolled_view_get(ETK_TREE(wd->tree));
    etk_scrolled_view_dragable_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_TRUE);
    etk_scrolled_view_drag_bouncy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_FALSE);
    etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_POLICY_HIDE, ETK_POLICY_SHOW);
    etk_scrolled_view_drag_damping_set(ETK_SCROLLED_VIEW(scrolled_view), 800);

    wd->container = etk_embed_new(e);
    etk_container_add(ETK_CONTAINER(wd->container), wd->tree);
    etk_widget_show_all(wd->container);
    Evas_Object *object = etk_embed_object_get(ETK_EMBED(wd->container));

    ogsmd_sim_retrieve_phonebook("contacts", _retrieve_callback, wd->widget_pointer);

   elm_widget_resize_object_set(wd->widget, object);
   //evas_object_smart_callback_add(wd->widget, "sub-object-del", _sub_del, wd->widget);
   
   _sizing_eval(wd->widget);
   return wd->widget;
}

EAPI GHashTable *
elm_my_contactlist_selected_row_get(void *data)
{
    Widget_Data *wd = elm_widget_data_get(data);
    Etk_Tree_Row *row = etk_tree_selected_row_get(wd->tree);
    if(row != NULL) {
        GHashTable *parameters = etk_tree_row_data_get(row);
        return parameters;
    }
    return NULL;
}

EAPI void
elm_my_contactlist_refresh(void *data)
{
    Widget_Data *wd = elm_widget_data_get(data);
    etk_tree_clear(wd->tree);
    ogsmd_sim_retrieve_phonebook("contacts", _retrieve_callback, wd->widget_pointer);
}

