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
#include <glib/gi18n.h>
#include <libebook/e-book-view.h>
#include <libedataserver/e-data-server-util.h>

#include "hito-group-store.h"
#include "hito-group.h"
#include "hito-category-group.h"

G_DEFINE_TYPE (HitoGroupStore, hito_group_store, GTK_TYPE_LIST_STORE);

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), HITO_TYPE_GROUP_STORE, HitoGroupStorePrivate))

typedef struct
{
  EBookView *view;
  /* A hash of category names to GtkTreeIter pointing at the relevant HitoGroup */
  GHashTable *name_hash;
  guint sig_added, sig_modified;
} HitoGroupStorePrivate;

enum {
  PROP_NULL,
  PROP_VIEW,
};

/*
 * Private methods.
 */

static void
on_contacts_added (EBookView *view, GList *contacts, HitoGroupStore *store)
{
  HitoGroupStorePrivate *priv = GET_PRIVATE (store);

  for (;contacts; contacts = g_list_next (contacts)) {
    EContact *contact = contacts->data;
    GList *categories, *l;
    GtkTreeIter iter;

    /* TODO: faster to get the string and tokenise it ourself? */
    categories = e_contact_get (contact, E_CONTACT_CATEGORY_LIST);

    for (l = categories; l; l = l->next) {
      const char *name = l->data;
      HitoGroup *group;
      
      if (!g_hash_table_lookup (priv->name_hash, name)) {
        group = hito_category_group_new (name);
        gtk_list_store_insert_with_values (GTK_LIST_STORE (store), &iter, 0,
                                           COL_GROUP, group, -1);
        g_hash_table_insert (priv->name_hash, g_strdup (name), gtk_tree_iter_copy (&iter));
      }
    }

    g_list_foreach (categories, (GFunc)g_free, NULL);
    g_list_free (categories);
  }
}

static gint
sorter_cb (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data)
{
  HitoGroup *group_a, *group_b;
  int res, weight_a, weight_b;

  gtk_tree_model_get (model, a, COL_GROUP, &group_a, -1);
  gtk_tree_model_get (model, b, COL_GROUP, &group_b, -1);

  weight_a = hito_group_get_weight (group_a);
  weight_b = hito_group_get_weight (group_b);

  if (weight_a != weight_b) {
    res = weight_a - weight_b;
    goto done;
  }
  
  res = g_utf8_collate (hito_group_get_name (group_a) ?: "",
                        hito_group_get_name (group_b) ?: "");

 done:
  g_object_unref (group_a);
  g_object_unref (group_b);

  return res;
}


/*
 * Object methods.
 */

static void
hito_group_store_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
  HitoGroupStorePrivate *priv = GET_PRIVATE (object);

  switch (property_id) {
  case PROP_VIEW:
    g_value_set_object (value, priv->view);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
hito_group_store_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
  switch (property_id) {
  case PROP_VIEW:
    hito_group_store_set_view (HITO_GROUP_STORE (object),
                               g_value_get_object (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
hito_group_store_dispose (GObject *object)
{
  hito_group_store_set_view (HITO_GROUP_STORE (object), NULL);

  if (G_OBJECT_CLASS (hito_group_store_parent_class)->dispose)
    G_OBJECT_CLASS (hito_group_store_parent_class)->dispose (object);
}

static void
hito_group_store_finalize (GObject *object)
{
  HitoGroupStorePrivate *priv = GET_PRIVATE (object);

  g_hash_table_destroy (priv->name_hash);
  
  G_OBJECT_CLASS (hito_group_store_parent_class)->finalize (object);
}

static void
hito_group_store_class_init (HitoGroupStoreClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (HitoGroupStorePrivate));

  object_class->get_property = hito_group_store_get_property;
  object_class->set_property = hito_group_store_set_property;
  object_class->dispose = hito_group_store_dispose;
  object_class->finalize = hito_group_store_finalize;

  g_object_class_install_property (object_class, PROP_VIEW,
                                   g_param_spec_object ("view", "view", NULL,
                                                        E_TYPE_BOOK_VIEW,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK));
}

static void
hito_group_store_init (HitoGroupStore *self)
{
  const GType column_types[] = { HITO_TYPE_GROUP };
  HitoGroupStorePrivate *priv = GET_PRIVATE (self);

  gtk_list_store_set_column_types (GTK_LIST_STORE (self),
                                   G_N_ELEMENTS (column_types),
                                   (GType *) column_types);

  gtk_tree_sortable_set_default_sort_func (GTK_TREE_SORTABLE (self),
                                           sorter_cb, NULL, NULL);
  gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (self),
                                        GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID,
                                        GTK_SORT_ASCENDING);

  priv->name_hash = g_hash_table_new_full (g_str_hash, g_str_equal,
                                      g_free, (GDestroyNotify)gtk_tree_iter_free);
}


/*
 * Public methods.
 */

GtkTreeModel *
hito_group_store_new (void)
{
  return g_object_new (HITO_TYPE_GROUP_STORE, NULL);
}

void
hito_group_store_set_view (HitoGroupStore *store, EBookView *view)
{
  HitoGroupStorePrivate *priv;

  g_return_if_fail (HITO_IS_GROUP_STORE (store));

  priv = GET_PRIVATE (store);

  if (priv->view) {
    g_signal_handler_disconnect (priv->view, priv->sig_added);
    g_signal_handler_disconnect (priv->view, priv->sig_modified);
    priv->sig_added = priv->sig_modified = 0;
    g_object_unref (priv->view);
    priv->view = NULL;
  }
  
  if (view) {
    priv->view = g_object_ref (view);
    priv->sig_added = g_signal_connect (priv->view, "contacts-added", G_CALLBACK (on_contacts_added), store);
    priv->sig_modified = g_signal_connect (priv->view, "contacts-changed", G_CALLBACK (on_contacts_added), store);
  }
}

void
hito_group_store_add_group (HitoGroupStore *store, HitoGroup *group)
{
  HitoGroupStorePrivate *priv;
  GtkTreeIter iter;
  
  g_return_if_fail (HITO_IS_GROUP_STORE (store));
  g_return_if_fail (HITO_IS_GROUP (group));

  priv = GET_PRIVATE (store);

  gtk_list_store_insert_with_values (GTK_LIST_STORE (store), &iter, 0,
                                     COL_GROUP, group, -1);

  if (HITO_IS_CATEGORY_GROUP (group)) {
    g_hash_table_insert (priv->name_hash,
                         g_strdup (hito_group_get_name (group)),
                         gtk_tree_iter_copy (&iter));
  }
}

static gboolean
find_group (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
  struct {
    HitoGroup *group;
    gboolean found;
    GtkTreeIter iter;
  } *data = user_data;
  HitoGroup *group = NULL;

  gtk_tree_model_get (model, iter, COL_GROUP, &group, -1);
  
  if (group == data->group) {
    data->found = TRUE;
    data->iter = *iter;
  }

  g_object_unref (group);

  return data->found;
}

void
hito_group_store_remove_group (HitoGroupStore *store, HitoGroup *group)
{
  HitoGroupStorePrivate *priv;
  struct {
    HitoGroup *group;
    gboolean found;
    GtkTreeIter iter;
  } data;

  g_return_if_fail (HITO_IS_GROUP_STORE (store));
  g_return_if_fail (HITO_IS_GROUP (group));

  priv = GET_PRIVATE (store);

  data.group = group;
  data.found = FALSE;
  
  gtk_tree_model_foreach (GTK_TREE_MODEL (store), find_group, &data);

  if (data.found) {
    gtk_list_store_remove (GTK_LIST_STORE (store), &data.iter);
    
    if (HITO_IS_CATEGORY_GROUP (group)) {
      g_hash_table_remove (priv->name_hash, hito_group_get_name (group));
    }
  }
}
