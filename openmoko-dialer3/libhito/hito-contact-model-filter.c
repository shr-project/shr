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
#include <libedataserver/e-data-server-util.h>

#include "hito-contact-store.h"
#include "hito-contact-model-filter.h"
#include "hito-all-group.h"

G_DEFINE_TYPE (HitoContactModelFilter,
               hito_contact_model_filter,
               GTK_TYPE_TREE_MODEL_FILTER);

#define GET_PRIVATE(o)                                                  \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), HITO_TYPE_CONTACT_MODEL_FILTER, HitoContactModelFilterPrivate))

typedef struct {
  HitoGroup *group;
  char *text;
} HitoContactModelFilterPrivate;

static gboolean
visible_func (GtkTreeModel *model, GtkTreeIter *iter, gpointer data)
{
  HitoContactModelFilterPrivate *priv = GET_PRIVATE (data);
  EContact *contact;
  gboolean visible = TRUE;

  gtk_tree_model_get (model, iter, COLUMN_CONTACT, &contact, -1);
  if (!contact) {
    g_warning (G_STRLOC ": NULL contact in filter");
    return FALSE;
  }
  
  /* First filter on the group */
  if (priv->group) {
    /* TODO: would it be worth fast-pathing ALL_GROUP here? */
    visible = hito_group_includes_contact (priv->group, contact);

    if (!visible)
      goto done;
  }

  if (priv->text) {
    const char *name;

    /* TODO: get name from store */
    name = e_contact_get_const (contact, E_CONTACT_FULL_NAME);
    
    /* TODO: split search into parts etc. priv->text is already normalised */
    visible = e_util_utf8_strstrcasedecomp (name, priv->text) != NULL;
  }

 done:
  g_object_unref (contact);

  return visible;
}

static void
hito_contact_model_filter_dispose (GObject *object)
{
  HitoContactModelFilterPrivate *priv = GET_PRIVATE (object);

  if (priv->group) {
    g_object_unref (priv->group);
    priv->group = NULL;
  }
  
  G_OBJECT_CLASS (hito_contact_model_filter_parent_class)->dispose (object);
}

static void
hito_contact_model_filter_class_init (HitoContactModelFilterClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (HitoContactModelFilterPrivate));

  object_class->dispose = hito_contact_model_filter_dispose;
}

static void
hito_contact_model_filter_init (HitoContactModelFilter *self)
{
  gtk_tree_model_filter_set_visible_func
    (GTK_TREE_MODEL_FILTER (self), visible_func, self, NULL);
}

GtkTreeModel *
hito_contact_model_filter_new (HitoContactStore *model)
{
  return g_object_new (HITO_TYPE_CONTACT_MODEL_FILTER,
                       "child-model", model,
                       NULL);
}

void
hito_contact_model_filter_set_group (HitoContactModelFilter *filter, HitoGroup *group)
{
  HitoContactModelFilterPrivate *priv;

  g_return_if_fail (HITO_IS_CONTACT_MODEL_FILTER (filter));
  priv = GET_PRIVATE (filter);

  if (priv->group) {
    g_object_unref (priv->group);
    priv->group = NULL;
  }

  if (group) {
    priv->group = g_object_ref (group);
  }
  
  gtk_tree_model_filter_refilter  (GTK_TREE_MODEL_FILTER (filter));
}

void
hito_contact_model_filter_set_text (HitoContactModelFilter *filter, const char *text)
{
  HitoContactModelFilterPrivate *priv;

  g_return_if_fail (HITO_IS_CONTACT_MODEL_FILTER (filter));
  priv = GET_PRIVATE (filter);

  if (priv->text) {
    g_free (priv->text);
    priv->text = NULL;
  }

  /* Normalise now for faster matching later */
  if (text)
    priv->text = g_utf8_normalize (text, -1, G_NORMALIZE_ALL);

  gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER (filter));
}
