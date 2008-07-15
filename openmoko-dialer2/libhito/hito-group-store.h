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

#ifndef _HITO_GROUP_STORE
#define _HITO_GROUP_STORE

#include <libebook/e-book-view.h>
#include <gtk/gtkliststore.h>

#include "hito-group.h"

G_BEGIN_DECLS

#define HITO_TYPE_GROUP_STORE hito_group_store_get_type()

#define HITO_GROUP_STORE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  HITO_TYPE_GROUP_STORE, HitoGroupStore))

#define HITO_GROUP_STORE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  HITO_TYPE_GROUP_STORE, HitoGroupStoreClass))

#define HITO_IS_GROUP_STORE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  HITO_TYPE_GROUP_STORE))

#define HITO_IS_GROUP_STORE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  HITO_TYPE_GROUP_STORE))

#define HITO_GROUP_STORE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  HITO_TYPE_GROUP_STORE, HitoGroupStoreClass))

enum {
  COL_GROUP, /* HitoGroup* */
};

typedef struct {
  GtkListStore parent;
} HitoGroupStore;

typedef struct {
  GtkListStoreClass parent_class;
} HitoGroupStoreClass;

GType hito_group_store_get_type (void);

GtkTreeModel* hito_group_store_new (void);

void hito_group_store_set_view (HitoGroupStore *store, EBookView *view);

void hito_group_store_add_group (HitoGroupStore *store, HitoGroup *group);

void hito_group_store_remove_group (HitoGroupStore *store, HitoGroup *group);

G_END_DECLS

#endif /* _HITO_GROUP_STORE */
