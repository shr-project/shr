/*
 * Copyright (C) 2007 OpenedHand Ltd
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <config.h>
#include <gtk/gtk.h>
#include "hito-group-store.h"
#include "hito-group-combo.h"
#include "hito-separator-group.h"

G_DEFINE_TYPE (HitoGroupCombo, hito_group_combo, GTK_TYPE_COMBO_BOX);

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), HITO_TYPE_GROUP_COMBO, HitoGroupComboPrivate))

typedef struct {
  guint changed_id;
  HitoContactModelFilter *filter;
} HitoGroupComboPrivate;

/*
 * Private methods.
 */

static gboolean
is_separator_func (GtkTreeModel *model, GtkTreeIter *iter, gpointer data)
{
  HitoGroup *group = NULL;

  gtk_tree_model_get (model, iter, COL_GROUP, &group, -1);

  return HITO_IS_SEPARATOR_GROUP (group);
}

static void
group_name_data_func (GtkCellLayout *layout, GtkCellRenderer *cell,
                      GtkTreeModel *model, GtkTreeIter *iter,
                      gpointer user_data)
{
  HitoGroup *group;

  gtk_tree_model_get (model, iter, COL_GROUP, &group, -1);
  if (!group)
    return;

  g_object_set (cell, "text", hito_group_get_name (group), NULL);

  g_object_unref (group);
}


static void
on_group_changed (HitoGroupCombo *combo)
{
  HitoGroupComboPrivate *priv = GET_PRIVATE (combo);
  HitoGroup *group;

  group = hito_group_combo_get_active_group (combo);

  hito_contact_model_filter_set_group (priv->filter, group);
}

/*
 * Object methods.
 */

static void
hito_group_combo_dispose (GObject *object)
{
  hito_group_combo_connect_filter (HITO_GROUP_COMBO (object), NULL);

  if (G_OBJECT_CLASS (hito_group_combo_parent_class)->dispose)
    G_OBJECT_CLASS (hito_group_combo_parent_class)->dispose (object);
}


static void
hito_group_combo_class_init (HitoGroupComboClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (HitoGroupComboPrivate));

  object_class->dispose = hito_group_combo_dispose;
}

static void
hito_group_combo_init (HitoGroupCombo *self)
{
  GtkCellRenderer *renderer;
  
  renderer = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (self), renderer, TRUE);
  gtk_cell_layout_set_cell_data_func  (GTK_CELL_LAYOUT (self), renderer,
                                       group_name_data_func, NULL, NULL);

  gtk_combo_box_set_row_separator_func (GTK_COMBO_BOX (self),
                                        is_separator_func, NULL, NULL);
}


/*
 * Public methods.
 */

GtkWidget *
hito_group_combo_new (HitoGroupStore *store)
{
  return g_object_new (HITO_TYPE_GROUP_COMBO,
                       "model", store,
                       NULL);
}

HitoGroup *
hito_group_combo_get_active_group (HitoGroupCombo *combo)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  HitoGroup *group = NULL;
  
  g_return_val_if_fail (HITO_IS_GROUP_COMBO (combo), NULL);
  
  if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (combo), &iter)) {
    model = gtk_combo_box_get_model (GTK_COMBO_BOX (combo));
    
    gtk_tree_model_get (model, &iter, COL_GROUP, &group, -1);
    
    return group;
  }
  
  return NULL;
}

/**
 * hito_group_combo_connect_filter:
 * @combo: A #HitoGroupCombo
 * @filter: A #HitoContactModelFilter, or #NULL.
 *
 * Hook the selected group in the combo box to the filter model.  If @filter is
 * #NULL, remove an existing connection.
 */
void
hito_group_combo_connect_filter (HitoGroupCombo *combo, HitoContactModelFilter *filter)
{
  HitoGroupComboPrivate *priv;

  g_return_if_fail (HITO_IS_GROUP_COMBO (combo));
  
  priv = GET_PRIVATE (combo);

  if (priv->changed_id) {
    g_signal_handler_disconnect (combo, priv->changed_id);
    g_object_unref (priv->filter);
    
    priv->changed_id = 0;
    priv->filter = NULL;
  }
  
  if (filter) {
    priv->changed_id = g_signal_connect (combo, "changed",
                                         G_CALLBACK (on_group_changed), NULL);
    priv->filter = g_object_ref (filter);
    on_group_changed (combo);
  }
}
