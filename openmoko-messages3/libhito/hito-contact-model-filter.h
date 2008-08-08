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

#ifndef _HITO_CONTACT_MODEL_FILTER
#define _HITO_CONTACT_MODEL_FILTER

#include <gtk/gtktreemodelfilter.h>
#include "hito-contact-store.h"
#include "hito-group.h"

G_BEGIN_DECLS

#define HITO_TYPE_CONTACT_MODEL_FILTER hito_contact_model_filter_get_type()

#define HITO_CONTACT_MODEL_FILTER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  HITO_TYPE_CONTACT_MODEL_FILTER, HitoContactModelFilter))

#define HITO_CONTACT_MODEL_FILTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  HITO_TYPE_CONTACT_MODEL_FILTER, HitoContactModelFilterClass))

#define HITO_IS_CONTACT_MODEL_FILTER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  HITO_TYPE_CONTACT_MODEL_FILTER))

#define HITO_IS_CONTACT_MODEL_FILTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  HITO_TYPE_CONTACT_MODEL_FILTER))

#define HITO_CONTACT_MODEL_FILTER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  HITO_TYPE_CONTACT_MODEL_FILTER, HitoContactModelFilterClass))

typedef struct {
  GtkTreeModelFilter parent;
} HitoContactModelFilter;

typedef struct {
  GtkTreeModelFilterClass parent_class;
} HitoContactModelFilterClass;

GType hito_contact_model_filter_get_type (void);

GtkTreeModel* hito_contact_model_filter_new (HitoContactStore *model);

void hito_contact_model_filter_set_group (HitoContactModelFilter *filter, HitoGroup *group);

void hito_contact_model_filter_set_text (HitoContactModelFilter *filter, const char *text);

G_END_DECLS

#endif /* _HITO_CONTACT_MODEL_FILTER */
