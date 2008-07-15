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
#include "hito-all-group.h"

G_DEFINE_TYPE (HitoAllGroup, hito_all_group, HITO_TYPE_GROUP);

static const char *
get_name (HitoGroup *group)
{
  return _("All");
}

static int
get_weight (HitoGroup *group)
{
  return -100;
}

static gboolean
includes_contact (HitoGroup *group, EContact *contact)
{
  return TRUE;
}

static void
hito_all_group_class_init (HitoAllGroupClass *klass)
{
  HitoGroupClass *group_class = HITO_GROUP_CLASS (klass);

  group_class->get_name = get_name;
  group_class->get_weight = get_weight;
  group_class->includes_contact = includes_contact;
}

static void
hito_all_group_init (HitoAllGroup *self)
{
}

HitoGroup *
hito_all_group_new (void)
{
  return g_object_new (HITO_TYPE_ALL_GROUP, NULL);
}
