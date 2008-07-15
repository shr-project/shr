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
#include <string.h>
#include <libebook/e-book.h>
#include "hito-contact-store.h"

G_DEFINE_TYPE (HitoContactStore, hito_contact_store, GTK_TYPE_LIST_STORE);

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), HITO_TYPE_CONTACT_STORE, HitoContactStorePrivate))

typedef struct _HitoContactStorePrivate HitoContactStorePrivate;

struct _HitoContactStorePrivate
{
  EBookView *view;
  guint sig_added, sig_modified, sig_removed;
  GHashTable *uid_hash;
};

enum {
  PROP_0,
  PROP_VIEW,
};

/*
 * Private methods
 */

static gint
sorter_cb (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data)
{
  int res;
  EContact *contact_a, *contact_b;
  const gchar *name_a = NULL, *name_b = NULL;
  
  gtk_tree_model_get (model, a, COLUMN_CONTACT, &contact_a, -1);
  gtk_tree_model_get (model, b, COLUMN_CONTACT, &contact_b, -1);

  name_a = e_contact_get_const (contact_a, E_CONTACT_FULL_NAME);
  name_b = e_contact_get_const (contact_b, E_CONTACT_FULL_NAME);

  if (name_a == name_b) {
    /* i.e. probably both null */
    return 0;
  }

  if (!name_a) {
    return -1;
  }
  if (!name_b) {
    return 1;
  }


  /* TODO: have a display name method, and sort on that */
  res = g_utf8_collate (name_a, name_b);

  g_object_unref (contact_a);
  g_object_unref (contact_b);
  
  return res;
}

/*
 * Callback when contacts in the book view are added.
 */
static void
on_contacts_added (EBookView *view, GList *contacts, HitoContactStore *store)
{
  HitoContactStorePrivate *priv = GET_PRIVATE (store);

  for (;contacts; contacts = g_list_next (contacts)) {
    EContact *contact;
    GtkTreeIter iter;

    contact = contacts->data;
    
    gtk_list_store_insert_with_values (GTK_LIST_STORE (store), &iter, 0,
                                       COLUMN_CONTACT, contact,
                                       -1);
    
    g_hash_table_insert (priv->uid_hash,
                         e_contact_get (contact, E_CONTACT_UID),
                         gtk_tree_iter_copy (&iter));
  }
}

/*
 * Callback when contacts in the calendar view are modified.
 */
static void
on_contacts_modified (EBookView *view, GList *contacts, HitoContactStore *store)
{
  HitoContactStorePrivate *priv = GET_PRIVATE (store);

  for (;contacts; contacts = g_list_next (contacts)) {
    EContact *contact;
    GtkTreeIter *iter;

    contact = contacts->data;

    iter = g_hash_table_lookup (priv->uid_hash, e_contact_get_const (contact, E_CONTACT_UID));

    gtk_list_store_set (GTK_LIST_STORE (store), iter, COLUMN_CONTACT, contact, -1);
  }
}

/*
 * Callback when contacts in the view are removed.
 */
static void
on_contacts_removed (EBookView *view, GList *uids, HitoContactStore *store) {
  HitoContactStorePrivate *priv = GET_PRIVATE (store);

  for (; uids; uids = g_list_next (uids)) {
    GtkTreeIter *iter;
    const char *uid = uids->data;

    iter = g_hash_table_lookup (priv->uid_hash, uid);
    if (iter) {
      gtk_list_store_remove (GTK_LIST_STORE (store), iter);
      g_hash_table_remove (priv->uid_hash, uid);
    } else {
      g_warning ("Cannot find iter for removed UID %s", uid);
    }
  }
}

/*
 * Object methods
 */

static void
hito_contact_store_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
  HitoContactStorePrivate *priv = GET_PRIVATE (object);

  switch (property_id) {
  case PROP_VIEW:
    g_value_set_object (value, priv->view);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
hito_contact_store_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
  switch (property_id) {
  case PROP_VIEW:
    hito_contact_store_set_view (HITO_CONTACT_STORE (object),
                               g_value_get_object (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
hito_contact_store_dispose (GObject *object)
{
  hito_contact_store_set_view (HITO_CONTACT_STORE (object), NULL);

  if (G_OBJECT_CLASS (hito_contact_store_parent_class)->dispose)
    G_OBJECT_CLASS (hito_contact_store_parent_class)->dispose (object);
}

static void
hito_contact_store_finalize (GObject *object)
{
  HitoContactStorePrivate *priv = GET_PRIVATE (object);

  g_hash_table_destroy (priv->uid_hash);
  
  G_OBJECT_CLASS (hito_contact_store_parent_class)->finalize (object);
}

static void
hito_contact_store_class_init (HitoContactStoreClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (HitoContactStorePrivate));

  object_class->get_property = hito_contact_store_get_property;
  object_class->set_property = hito_contact_store_set_property;
  object_class->dispose = hito_contact_store_dispose;
  object_class->finalize = hito_contact_store_finalize;

  g_object_class_install_property (object_class, PROP_VIEW,
                                   g_param_spec_object ("view", "view", NULL,
                                                        E_TYPE_BOOK_VIEW,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK));
}

static void
hito_contact_store_init (HitoContactStore *self)
{
  HitoContactStorePrivate *priv = GET_PRIVATE (self);

  const GType column_types[] = {
    E_TYPE_CONTACT,
  };

  gtk_list_store_set_column_types (GTK_LIST_STORE (self),
                                   G_N_ELEMENTS (column_types),
                                   (GType *) column_types);

  gtk_tree_sortable_set_default_sort_func (GTK_TREE_SORTABLE (self),
                                           sorter_cb, NULL, NULL);
  gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (self),
                                        GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID,
                                        GTK_SORT_ASCENDING);

  priv->uid_hash = g_hash_table_new_full (g_str_hash, g_str_equal,
                                          g_free, (GDestroyNotify)gtk_tree_iter_free);
}


/*
 * Public methods
 */

GtkTreeModel*
hito_contact_store_new (EBookView *view)
{
  return g_object_new (HITO_TYPE_CONTACT_STORE,
                       "view", view,
                       NULL);
}

void
hito_contact_store_set_view (HitoContactStore *store, EBookView *view)
{
  HitoContactStorePrivate *priv;

  g_return_if_fail (HITO_IS_CONTACT_STORE (store));

  priv = GET_PRIVATE (store);

  if (priv->view) {
    g_signal_handler_disconnect (priv->view, priv->sig_added);
    g_signal_handler_disconnect (priv->view, priv->sig_modified);
    g_signal_handler_disconnect (priv->view, priv->sig_removed);
    priv->sig_added = priv->sig_modified = priv->sig_removed = 0;
    
    g_object_unref (priv->view);
    priv->view = NULL;
  }
  
  if (view) {
    priv->view = g_object_ref (view);
    priv->sig_added = g_signal_connect (priv->view, "contacts-added", G_CALLBACK (on_contacts_added), store);
    priv->sig_modified = g_signal_connect (priv->view, "contacts-changed", G_CALLBACK (on_contacts_modified), store);
    priv->sig_removed = g_signal_connect (priv->view, "contacts-removed", G_CALLBACK (on_contacts_removed), store);
  }
}

gboolean
hito_contact_store_get_iter_for_uid (HitoContactStore *store, const char *uid, GtkTreeIter *iter)
{
  HitoContactStorePrivate *priv;
  GtkTreeIter *it;

  g_return_val_if_fail (HITO_IS_CONTACT_STORE (store), FALSE);
  g_return_val_if_fail (uid, FALSE);
  g_return_val_if_fail (iter, FALSE);

  priv = GET_PRIVATE (store);

  if (g_hash_table_lookup_extended (priv->uid_hash, uid, NULL, (gpointer*)(void*)&it)) {
    *iter = *it;
    return TRUE;
  } else {
    return FALSE;
  }
}
