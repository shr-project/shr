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
#include <gtk/gtk.h>
#include <libebook/e-book.h>
#include "hito-contact-store.h"
#include "hito-contact-model-filter.h"
#include "hito-contact-view.h"
#include "hito-contact-preview.h"
#include "hito-group-store.h"
#include "hito-group-combo.h"
#include "hito-all-group.h"
#include "hito-category-group.h"
#include "hito-no-category-group.h"
#include "hito-separator-group.h"


static EBook *book;
static EBookView *view = NULL;

static void
on_entry_changed (GtkEntry *entry, HitoContactModelFilter *filter)
{
  hito_contact_model_filter_set_text (filter, gtk_entry_get_text (entry));
}

static void
on_selection_changed (GtkTreeSelection *selection, HitoContactPreview *preview)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  EContact *contact;

  if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
    gtk_tree_model_get (model, &iter, COLUMN_CONTACT, &contact, -1);
    hito_contact_preview_set_contact (preview, contact);
    g_object_unref (contact);
  } else {
    hito_contact_preview_set_contact (preview, NULL);
  }
}

static void
make_contact_view (void)
{
  GtkWidget *window, *box, *combo, *scrolled, *treeview, *entry, *preview;
  GtkTreeModel *group_store, *contact_store, *contact_filter;

  group_store = hito_group_store_new ();
  hito_group_store_set_view (HITO_GROUP_STORE (group_store), view);
  hito_group_store_add_group (HITO_GROUP_STORE (group_store), hito_all_group_new ());
  hito_group_store_add_group (HITO_GROUP_STORE (group_store), hito_separator_group_new (-99));
  hito_group_store_add_group (HITO_GROUP_STORE (group_store), hito_separator_group_new (99));
  hito_group_store_add_group (HITO_GROUP_STORE (group_store), hito_no_category_group_new ());
  
  contact_store = hito_contact_store_new (view);
  contact_filter = hito_contact_model_filter_new (HITO_CONTACT_STORE (contact_store));

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "delete-event", G_CALLBACK (gtk_main_quit), NULL);
  gtk_window_set_title (GTK_WINDOW (window), "Contacts");
  gtk_window_set_default_size (GTK_WINDOW (window), 300, 300);
  gtk_container_set_border_width (GTK_CONTAINER (window), 8);
  
  box = gtk_vbox_new (FALSE, 8);
  gtk_container_add (GTK_CONTAINER (window), box);

  combo = hito_group_combo_new (HITO_GROUP_STORE (group_store));
  hito_group_combo_connect_filter (HITO_GROUP_COMBO (combo),
                                   HITO_CONTACT_MODEL_FILTER (contact_filter));
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
  gtk_box_pack_start (GTK_BOX (box), combo, FALSE, FALSE, 0);

  scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_box_pack_start (GTK_BOX (box), scrolled, TRUE, TRUE, 0);

  treeview = hito_contact_view_new (HITO_CONTACT_STORE (contact_store),
                                    HITO_CONTACT_MODEL_FILTER (contact_filter));
  gtk_container_add (GTK_CONTAINER (scrolled), treeview);

  entry = gtk_entry_new ();
  g_signal_connect (entry, "changed", G_CALLBACK (on_entry_changed), contact_filter);
  gtk_box_pack_start (GTK_BOX (box), entry, FALSE, FALSE, 0);

  preview = hito_contact_preview_new ();
  g_signal_connect (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview)),
                    "changed", G_CALLBACK (on_selection_changed), preview);
  gtk_box_pack_start (GTK_BOX (box), preview, TRUE, TRUE, 0);
  
  gtk_widget_show_all (window);
}

int
main (int argc, char **argv)
{
  EBookQuery *query;

  g_thread_init (NULL);
  gtk_init (&argc, &argv);

  book = e_book_new_system_addressbook (NULL);
  g_assert (book);

  e_book_open (book, FALSE, NULL);

  query = e_book_query_any_field_contains (NULL);

  e_book_get_book_view (book, query, NULL, 0, &view, NULL);
  g_assert (view);
  e_book_query_unref (query);
  
  e_book_view_start (view);

  make_contact_view ();
  
  gtk_main ();

  e_book_view_stop (view);
  g_object_unref (book);
  
  return 0;
}
