/*
 *  moko-progressbar-window.c: Display a window with a progressbar while sending a sms
 *
 *  Authored by Andreas Engelbredt Dalsgaard <andreas.dalsgaard@gmail.com>
 *
 *  Copyright (C) 2008 Andreas Engelbredt Dalsgaard
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser Public License as published by
 *  the Free Software Foundation; version 2 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser Public License for more details.
 *
 */


#include <gtk/gtk.h>

static GtkWidget *window;
static gboolean status = FALSE;
static GError *error = NULL;

void moko_close_progressbar_window (GError *err)
{
	error = err;
	status = TRUE;
}

void moko_progressbar_window ()
{
	GtkWidget *progressbar, *vbox;
	
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_type_hint (GTK_WINDOW (window), GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_title (GTK_WINDOW (window), "Please wait");

	vbox = gtk_vbox_new (TRUE, 0);
	gtk_container_add (GTK_CONTAINER (window), vbox);
	  
	progressbar = gtk_progress_bar_new ();
	gtk_box_pack_start_defaults (GTK_BOX (vbox), progressbar);

	gtk_widget_show_all (window);
	
	while (status == FALSE)
	{
		gtk_progress_bar_pulse(GTK_PROGRESS_BAR(progressbar));
	  	g_main_context_iteration (NULL, FALSE);
	}
	
	if (error != NULL)	
	{
		//TODO display error dialog
		g_debug ("Error sending message: %s", error->message);
	}
	
	gtk_widget_destroy (window);
}

