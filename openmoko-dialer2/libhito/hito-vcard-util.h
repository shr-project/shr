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

#ifndef HITO_VCARD_UTIL_H
#define HITO_VCARD_UTIL_H

#include <libebook/e-contact.h>

GList *  hito_vcard_get_named_attributes (EVCard *contact, const char *name);
void     hito_vcard_attribute_set_type (EVCardAttribute *attr, const gchar *type);
gchar*   hito_vcard_attribute_get_type (EVCardAttribute *attr);
gchar*   hito_vcard_attribute_get_value_string (EVCardAttribute *attr);
gboolean hito_vcard_attribute_compare_value_strings (gchar *a, gchar *b);
gboolean hito_vcard_attribute_has_value (EVCardAttribute *card, const gchar *value);
GdkPixbuf* hito_vcard_get_photo_pixbuf (EVCard *card);

gint hito_vcard_strip_empty_attributes (EVCard *card);

#endif /* HITO_VCARD_UTIL_H */
