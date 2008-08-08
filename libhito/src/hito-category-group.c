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
#include "hito-category-group.h"

G_DEFINE_TYPE (HitoCategoryGroup, hito_category_group, HITO_TYPE_GROUP);

#define GET_PRIVATE(o)                                                  \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), HITO_TYPE_CATEGORY_GROUP, HitoCategoryGroupPrivate))

typedef struct {
  char *name;
} HitoCategoryGroupPrivate;

static const char *
get_name (HitoGroup *group)
{
  g_return_val_if_fail (HITO_IS_CATEGORY_GROUP (group), NULL);
  
  return GET_PRIVATE (group)->name;
}

static int
get_weight (HitoGroup *group)
{
  return 0;
}

static gboolean
includes_contact (HitoGroup *group, EContact *contact)
{
  HitoCategoryGroupPrivate *priv;
  GList *categories, *l;
  gboolean visible = FALSE;

  g_return_val_if_fail (HITO_IS_CATEGORY_GROUP (group), FALSE);
  g_return_val_if_fail (E_IS_CONTACT (contact), FALSE);

  priv = GET_PRIVATE (group);

  categories = e_contact_get (contact, E_CONTACT_CATEGORY_LIST);
  for (l = categories; l ; l = l->next) {
    /* TODO: strmp? or funky decomposition compare? */
    if (strcmp (priv->name, l->data) == 0) {
      visible = TRUE;
      break;
    }
  }
  g_list_foreach (categories, (GFunc)g_free, NULL);
  g_list_free (categories);

  return visible;
}

static void
hito_category_group_class_init (HitoCategoryGroupClass *klass)
{
  HitoGroupClass *group_class = HITO_GROUP_CLASS (klass);

  g_type_class_add_private (klass, sizeof (HitoCategoryGroupPrivate));
  
  group_class->get_name = get_name;
  group_class->get_weight = get_weight;
  group_class->includes_contact = includes_contact;
}

static void
hito_category_group_init (HitoCategoryGroup *self)
{
}

HitoGroup *
hito_category_group_new (const char *name)
{
  HitoGroup *group;

  g_return_val_if_fail (name, NULL);

  group = g_object_new (HITO_TYPE_CATEGORY_GROUP, NULL);
  /* TODO: set a property */
  GET_PRIVATE (group)->name = g_strdup (name);

  return group;
}
