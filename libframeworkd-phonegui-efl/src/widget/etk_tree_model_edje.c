/*  Copyright (C) 2006-2008 Davide Andreoli (see AUTHORS)
 *
 *  This file is part of Edje_editor.
 *  Edje_editor is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Edje_editor is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Edje_editor.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "etk_tree_model_edje.h"
#include <string.h>
#include <Evas.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>


typedef struct Etk_Tree_Model_Edje {
	Etk_Tree_Model model;
	Etk_Cache *cache;
	const char *file;
	const char *part;
} Etk_Tree_Model_Edje;

typedef struct Etk_Tree_Model_Edje_Data {
	GHashTable *parameters;
} Etk_Tree_Model_Edje_Data;


static void _edje_model_free(Etk_Tree_Model * model);
static void _edje_cell_data_free(Etk_Tree_Model * model, void *cell_data);
static void _edje_cell_data_set(Etk_Tree_Model * model, void *cell_data,
				va_list * args);
static void _edje_cell_data_get(Etk_Tree_Model * model, void *cell_data,
				va_list * args);
static void _edje_objects_cache(Etk_Tree_Model * model, void *cell_data,
				Evas_Object *
				cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL]);
static Etk_Bool _edje_render(Etk_Tree_Model * model, Etk_Tree_Row * row,
			     Etk_Geometry geometry, void *cell_data,
			     Evas_Object *
			     cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL],
			     Evas * evas);
static int _edje_width_get(Etk_Tree_Model * model, void *cell_data,
			   Evas_Object *
			   cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL]);
static void _edje_cache_remove(Etk_Tree_Model * model, const char *filename,
			       const char *key);


/* The function to create the model */
Etk_Tree_Model *
etk_tree_model_edje_new(const char *file, const char *part)
{
	Etk_Tree_Model *model;
	Etk_Tree_Model_Edje *edje_model;

	model = calloc(1, sizeof(Etk_Tree_Model_Edje));
	edje_model = (Etk_Tree_Model_Edje *) model;

	model->cell_data_size = sizeof(Etk_Tree_Model_Edje_Data);
	model->model_free = _edje_model_free;
	model->cell_data_free = _edje_cell_data_free;
	model->cell_data_set = _edje_cell_data_set;
	model->cell_data_get = _edje_cell_data_get;
	model->objects_cache = _edje_objects_cache;
	model->render = _edje_render;
	model->width_get = _edje_width_get;
	model->cache_remove = _edje_cache_remove;

	edje_model->cache = etk_cache_new(50);
	edje_model->file = file;
	edje_model->part = part;

	return model;
}

/* Edje: model_free() */
static void
_edje_model_free(Etk_Tree_Model * model)
{
	Etk_Tree_Model_Edje *edje_model;

	if (!(edje_model = (Etk_Tree_Model_Edje *) model))
		return;
	etk_cache_destroy(edje_model->cache);
}

/* Edje: cell_data_free() */
static void
_edje_cell_data_free(Etk_Tree_Model * model, void *cell_data)
{
	Etk_Tree_Model_Edje_Data *edje_data;

	if (!(edje_data = cell_data))
		return;

	//free(edje_data->parameters);
}

/* Edje: cell_data_set() */
static void
_edje_cell_data_set(Etk_Tree_Model * model, void *cell_data, va_list * args)
{
	Etk_Tree_Model_Edje_Data *edje_data;
	GHashTable *parameters;

	if (!(edje_data = cell_data) || !args || !model)
		return;

	/* Get the edje name from the args */
	parameters = va_arg(*args, GHashTable *);
	if (edje_data->parameters != parameters) {
		free(edje_data->parameters);

		/* TODO: edje_data->parameters = parameters ? copy(parameters) : NULL; // use g_boxed_copy? */
		edje_data->parameters = parameters ? parameters : NULL;
	}
}

/* Edje: cell_data_get() */
static void
_edje_cell_data_get(Etk_Tree_Model * model, void *cell_data, va_list * args)
{
	Etk_Tree_Model_Edje_Data *edje_data;
	GHashTable **parameters;

	if (!(edje_data = cell_data) || !args || !model)
		return;

	parameters = va_arg(*args, GHashTable **);
	if (parameters)
		*parameters = edje_data->parameters;
}

/* Edje: objects_cache() */
static void
_edje_objects_cache(Etk_Tree_Model * model, void *cell_data,
		    Evas_Object * cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL])
{
	Etk_Tree_Model_Edje *edje_model;
	Etk_Tree_Model_Edje_Data *edje_data;

	if (!(edje_model = (Etk_Tree_Model_Edje *) model) || !cell_objects[0])
		return;

	edje_data = cell_data;
	if (edje_data)
		etk_cache_add(edje_model->cache, cell_objects[0],
			      edje_data->parameters, NULL);
	else
		evas_object_del(cell_objects[0]);

	cell_objects[0] = NULL;
}


/* Edje: set_text_cb() */
void
_edje_set_text_cb(gpointer key, gpointer value, gpointer user_data)
{
	//g_debug("SET: %s", key);
	Evas_Object *cell_object = user_data;
	edje_object_part_text_set(cell_object, key, value);
}


/* Edje: render() */
static Etk_Bool
_edje_render(Etk_Tree_Model * model, Etk_Tree_Row * row, Etk_Geometry geometry,
	     void *cell_data,
	     Evas_Object * cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL],
	     Evas * evas)
{
	//g_debug("render called");

	Etk_Tree_Model_Edje *edje_model;
	Etk_Tree_Model_Edje_Data *edje_data;

	if (!(edje_model = (Etk_Tree_Model_Edje *) model) ||
	    !(edje_data = cell_data) || !evas || !edje_data->parameters)
		return ETK_FALSE;

	if (!
	    (cell_objects[0] =
	     etk_cache_find(edje_model->cache, edje_data->parameters, NULL))) {
		//g_debug("Rendering");

		cell_objects[0] = edje_object_add(evas);
		if (!cell_objects[0])
			return ETK_FALSE;
	}
	if (!edje_file_group_exists(edje_model->file, edje_model->part)) {
		g_error("%s does not exist in %s", edje_model->part,
			edje_model->file);
	}
	edje_object_file_set(cell_objects[0], edje_model->file,
			     edje_model->part);
	edje_extern_object_min_size_set(cell_objects[0], geometry.w,
					geometry.h);
	g_hash_table_foreach(edje_data->parameters, _edje_set_text_cb,
			     cell_objects[0]);
	evas_object_move(cell_objects[0], geometry.x, geometry.y);

	evas_object_resize(cell_objects[0], geometry.w, geometry.h);
	evas_object_show(cell_objects[0]);

	return ETK_TRUE;
}

/* Edje: width_get() */
static int
_edje_width_get(Etk_Tree_Model * model, void *cell_data,
		Evas_Object * cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL])
{
	Etk_Tree_Model_Edje *edje_model;
	int w;

	if (!(edje_model = (Etk_Tree_Model_Edje *) model) || !cell_objects[0])
		return 0;

	evas_object_geometry_get(cell_objects[0], NULL, NULL, &w, NULL);
	return w;
}

/* Edje: delete a certain evas object from the model's cache */
static void
_edje_cache_remove(Etk_Tree_Model * model, const char *filename,
		   const char *key)
{
	Etk_Tree_Model_Edje *edje_model;

	if (!(edje_model = (Etk_Tree_Model_Edje *) model))
		return;

	etk_cache_remove(edje_model->cache,
			 etk_cache_find(edje_model->cache, filename, NULL));
}
