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

#ifndef _HITO_SEPARATOR_GROUP
#define _HITO_SEPARATOR_GROUP

#include "hito-group.h"

G_BEGIN_DECLS

#define HITO_TYPE_SEPARATOR_GROUP hito_separator_group_get_type()

#define HITO_SEPARATOR_GROUP(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  HITO_TYPE_SEPARATOR_GROUP, HitoSeparatorGroup))

#define HITO_SEPARATOR_GROUP_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  HITO_TYPE_SEPARATOR_GROUP, HitoSeparatorGroupClass))

#define HITO_IS_SEPARATOR_GROUP(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  HITO_TYPE_SEPARATOR_GROUP))

#define HITO_IS_SEPARATOR_GROUP_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  HITO_TYPE_SEPARATOR_GROUP))

#define HITO_SEPARATOR_GROUP_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  HITO_TYPE_SEPARATOR_GROUP, HitoSeparatorGroupClass))

typedef struct {
  HitoGroup parent;
} HitoSeparatorGroup;

typedef struct {
  HitoGroupClass parent_class;
} HitoSeparatorGroupClass;

GType hito_separator_group_get_type (void);

HitoGroup* hito_separator_group_new (int weight);

G_END_DECLS

#endif /* _HITO_SEPARATOR_GROUP */
