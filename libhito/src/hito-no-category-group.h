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

#ifndef _HITO_NO_CATEGORY_GROUP
#define _HITO_NO_CATEGORY_GROUP

#include "hito-group.h"

G_BEGIN_DECLS

#define HITO_TYPE_NO_CATEGORY_GROUP hito_no_category_group_get_type()

#define HITO_NO_CATEGORY_GROUP(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  HITO_TYPE_NO_CATEGORY_GROUP, HitoNoCategoryGroup))

#define HITO_NO_CATEGORY_GROUP_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  HITO_TYPE_NO_CATEGORY_GROUP, HitoNoCategoryGroupClass))

#define HITO_IS_NO_CATEGORY_GROUP(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  HITO_TYPE_NO_CATEGORY_GROUP))

#define HITO_IS_NO_CATEGORY_GROUP_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  HITO_TYPE_NO_CATEGORY_GROUP))

#define HITO_NO_CATEGORY_GROUP_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  HITO_TYPE_NO_CATEGORY_GROUP, HitoNoCategoryGroupClass))

typedef struct {
  HitoGroup parent;
} HitoNoCategoryGroup;

typedef struct {
  HitoGroupClass parent_class;
} HitoNoCategoryGroupClass;

GType hito_no_category_group_get_type (void);

HitoGroup* hito_no_category_group_new (void);

G_END_DECLS

#endif /* _HITO_NO_CATEGORY_GROUP */
