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

#ifndef _HITO_CONTACT_VIEW
#define _HITO_CONTACT_VIEW

#include <gtk/gtktreeview.h>
#include "hito-contact-model-filter.h"
#include "hito-contact-store.h"

G_BEGIN_DECLS

#define HITO_TYPE_CONTACT_VIEW hito_contact_view_get_type()

#define HITO_CONTACT_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  HITO_TYPE_CONTACT_VIEW, HitoContactView))

#define HITO_CONTACT_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  HITO_TYPE_CONTACT_VIEW, HitoContactViewClass))

#define HITO_IS_CONTACT_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  HITO_TYPE_CONTACT_VIEW))

#define HITO_IS_CONTACT_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  HITO_TYPE_CONTACT_VIEW))

#define HITO_CONTACT_VIEW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  HITO_TYPE_CONTACT_VIEW, HitoContactViewClass))

typedef struct {
  GtkTreeView parent;
} HitoContactView;

typedef struct {
  GtkTreeViewClass parent_class;
} HitoContactViewClass;

GType hito_contact_view_get_type (void);

GtkWidget * hito_contact_view_new (HitoContactStore *store, HitoContactModelFilter *filter);

#if 0
HitoContact * hito_contact_view_get_selected_contact (HitoContactView *view);
#endif

gboolean hito_contact_view_get_selected_iter (HitoContactView *view, GtkTreeIter *iter);

G_END_DECLS

#endif /* _HITO_CONTACT_VIEW */
