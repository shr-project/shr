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

#ifndef _HITO_CONTACT_STORE
#define _HITO_CONTACT_STORE

#include <gtk/gtkliststore.h>
#include <libebook/e-book-view.h>

G_BEGIN_DECLS

#define HITO_TYPE_CONTACT_STORE hito_contact_store_get_type()

#define HITO_CONTACT_STORE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  HITO_TYPE_CONTACT_STORE, HitoContactStore))

#define HITO_CONTACT_STORE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  HITO_TYPE_CONTACT_STORE, HitoContactStoreClass))

#define HITO_IS_CONTACT_STORE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  HITO_TYPE_CONTACT_STORE))

#define HITO_IS_CONTACT_STORE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  HITO_TYPE_CONTACT_STORE))

#define HITO_CONTACT_STORE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  HITO_TYPE_CONTACT_STORE, HitoContactStoreClass))

enum {
  COLUMN_CONTACT, /* EContact */
};

typedef struct {
  GtkListStore parent;
} HitoContactStore;

typedef struct {
  GtkListStoreClass parent_class;
} HitoContactStoreClass;

GType hito_contact_store_get_type (void);

GtkTreeModel* hito_contact_store_new (EBookView *view);

void hito_contact_store_set_view (HitoContactStore *store, EBookView *view);

gboolean hito_contact_store_get_iter_for_uid (HitoContactStore *store, const char *uid, GtkTreeIter *iter);

G_END_DECLS

#endif /* _HITO_CONTACT_STORE */
