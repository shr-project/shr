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

#ifndef _HITO_GROUP
#define _HITO_GROUP

#include <libebook/e-contact.h>

G_BEGIN_DECLS

#define HITO_TYPE_GROUP hito_group_get_type()

#define HITO_GROUP(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  HITO_TYPE_GROUP, HitoGroup))

#define HITO_GROUP_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  HITO_TYPE_GROUP, HitoGroupClass))

#define HITO_IS_GROUP(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  HITO_TYPE_GROUP))

#define HITO_IS_GROUP_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  HITO_TYPE_GROUP))

#define HITO_GROUP_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  HITO_TYPE_GROUP, HitoGroupClass))

typedef struct {
  GObject parent;
} HitoGroup;

typedef struct {
  GObjectClass parent_class;
  const char * (* get_name) (HitoGroup *group);
  int (* get_weight) (HitoGroup *group);
  gboolean (* includes_contact) (HitoGroup *group, EContact *contact);
} HitoGroupClass;

GType hito_group_get_type (void);

const char *hito_group_get_name (HitoGroup *group);

int hito_group_get_weight (HitoGroup *group);

gboolean hito_group_includes_contact (HitoGroup *group, EContact *contact);

G_END_DECLS

#endif /* _HITO_GROUP */
