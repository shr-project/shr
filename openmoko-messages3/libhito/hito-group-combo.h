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

#ifndef _HITO_GROUP_COMBO
#define _HITO_GROUP_COMBO

#include <gtk/gtkcombobox.h>
#include "hito-contact-model-filter.h"
#include "hito-group-store.h"

G_BEGIN_DECLS

#define HITO_TYPE_GROUP_COMBO hito_group_combo_get_type()

#define HITO_GROUP_COMBO(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  HITO_TYPE_GROUP_COMBO, HitoGroupCombo))

#define HITO_GROUP_COMBO_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  HITO_TYPE_GROUP_COMBO, HitoGroupComboClass))

#define HITO_IS_GROUP_COMBO(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  HITO_TYPE_GROUP_COMBO))

#define HITO_IS_GROUP_COMBO_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  HITO_TYPE_GROUP_COMBO))

#define HITO_GROUP_COMBO_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  HITO_TYPE_GROUP_COMBO, HitoGroupComboClass))

typedef struct {
  GtkComboBox parent;
} HitoGroupCombo;

typedef struct {
  GtkComboBoxClass parent_class;
} HitoGroupComboClass;

GType hito_group_combo_get_type (void);

GtkWidget* hito_group_combo_new (HitoGroupStore *store);

HitoGroup* hito_group_combo_get_active_group (HitoGroupCombo *combo);

void hito_group_combo_connect_filter (HitoGroupCombo *combo, HitoContactModelFilter *filter);

G_END_DECLS

#endif /* _HITO_GROUP_COMBO */
