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
#include "hito-separator-group.h"

G_DEFINE_TYPE (HitoSeparatorGroup, hito_separator_group, HITO_TYPE_GROUP);

#define GET_PRIVATE(o)                                                  \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), HITO_TYPE_SEPARATOR_GROUP, HitoSeparatorGroupPrivate))

typedef struct {
  int weight;
} HitoSeparatorGroupPrivate;

static const char *
get_name (HitoGroup *group)
{
  /* Need to implement this because GtkComboBox measures the height of
     separators rows */
  return NULL;
}

static int
get_weight (HitoGroup *group)
{
  g_return_val_if_fail (HITO_IS_SEPARATOR_GROUP (group), 0);

  return GET_PRIVATE (group)->weight;
}

static void
hito_separator_group_class_init (HitoSeparatorGroupClass *klass)
{
  HitoGroupClass *group_class = HITO_GROUP_CLASS (klass);

  g_type_class_add_private (klass, sizeof (HitoSeparatorGroupPrivate));
  
  group_class->get_name = get_name;
  group_class->get_weight = get_weight;
}

static void
hito_separator_group_init (HitoSeparatorGroup *self)
{
}

HitoGroup *
hito_separator_group_new (int weight)
{
  HitoGroup *group;

  group = g_object_new (HITO_TYPE_SEPARATOR_GROUP, NULL);
  /* TODO: set a property */
  GET_PRIVATE (group)->weight = weight;

  return group;
}
