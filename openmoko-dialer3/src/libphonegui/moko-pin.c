/*
 *  Authored by OpenedHand Ltd <info@openedhand.com>
 *
 *  Copyright (C) 2007 OpenMoko Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser Public License as published by
 *  the Free Software Foundation; version 2 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 */

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#include "moko-dialer-textview.h"
#include "moko-dialer-panel.h"
#include "sim.h"
#include "moko-pin.h"

static gboolean is_sim_code_gui_active = FALSE;

typedef struct
{
  GtkWidget *display;
  GtkWidget *dialog;
  gchar *code;
} MokoPinData;

void 
sim_display_code_UI ()
{
	int needed_code;
	GError *error = NULL;
	if (sim_get_authentication_state (&error, &needed_code))
  {
		if (!is_sim_code_gui_active && needed_code != SIM_READY)
    {
      get_sim_code_from_user (needed_code);
		}
	}
}

static void
on_pad_user_input (MokoDialerPanel *panel, const gchar digit,
                   MokoPinData *data)
{
  gchar buf[2];
  gchar *new_code;

  /* Phones use '#' for PIN 'entered' signal */
  if (digit == '#')
  {
    gtk_dialog_response (GTK_DIALOG (data->dialog), GTK_RESPONSE_OK);
    return;
  }
  
  /* "*" acts as delete for now */
  if (digit == '*')
  {
    moko_dialer_textview_delete (MOKO_DIALER_TEXTVIEW (data->display));
    data->code[strlen(data->code)-1] = '\0';
    return;
  }

  /* Create a string from the new digit */
  buf[0] = digit;
  buf[1] = '\0';
  
  if (!data->code)
  {
    new_code = g_strdup (buf);
  }
  else
  {
    new_code = g_strconcat (data->code, buf, NULL);
    g_free (data->code);
  }
  data->code = new_code;

  moko_dialer_textview_insert (MOKO_DIALER_TEXTVIEW (data->display), "*");
}

void
get_sim_code_from_user (const int initial_status)
{
  GtkWidget *pad;
  MokoPinData data;
  int current_status = initial_status;
  gboolean result = TRUE;
  GError *error = NULL;
  char *puk, *pin;

  /* Set a beacon around here to state that the GUI is active */
  is_sim_code_gui_active = TRUE;

  /* Build the GUI */
  data.dialog = gtk_dialog_new_with_buttons ("Enter PIN code", NULL, 0,
                                             GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                             GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
  gtk_dialog_set_has_separator (GTK_DIALOG (data.dialog), FALSE);

  data.display = moko_dialer_textview_new ();
  gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (data.dialog)->vbox), data.display);
  
  pad = moko_dialer_panel_new ();
  gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (data.dialog)->vbox), pad);
  g_signal_connect (pad, "user_input", G_CALLBACK (on_pad_user_input), &data);
  
  gtk_widget_show_all (GTK_DIALOG (data.dialog)->vbox);

  /* The PIN/PUK conversation */
  while (result || current_status != SIM_READY)
  {
    data.code = NULL;
    switch (current_status)
    {
      case SIM_PIN_REQUIRED:
        if (gtk_dialog_run (GTK_DIALOG (data.dialog)) == GTK_RESPONSE_OK)
        {
          result = sim_send_pin_code (&error, &current_status, data.code);
        }
      break;
      case SIM_PUK_REQUIRED:
        /* I need to set a new title to the dialog here */
        if (gtk_dialog_run (GTK_DIALOG (data.dialog)) == GTK_RESPONSE_OK)
        {
          puk = g_strdup (data.code);
        } else {
	  break;
	}
        /* I need to set a new title to the dialog here */
        if (gtk_dialog_run (GTK_DIALOG (data.dialog)) == GTK_RESPONSE_OK)
        {
          pin = g_strdup (data.code);
        } else {
	  g_free (puk);
	  break;
	}
        result = sim_send_puk_code (&error, &current_status, puk, pin);
        g_free (pin);
        g_free (puk);
      break;
      case SIM_PIN2_REQUIRED:
        if (gtk_dialog_run (GTK_DIALOG (data.dialog)) == GTK_RESPONSE_OK)
        {
          result = sim_send_pin_code (&error, &current_status, data.code);
        }
      break;
      case SIM_PUK2_REQUIRED:
        /* I need to set a new title to the dialog here */
        if (gtk_dialog_run (GTK_DIALOG (data.dialog)) == GTK_RESPONSE_OK)
        {
          puk = g_strdup (data.code);
        } else {
	  break;
	}
        /* I need to set a new title to the dialog here */
        if (gtk_dialog_run (GTK_DIALOG (data.dialog)) == GTK_RESPONSE_OK)
        {
          pin = g_strdup (data.code);
        } else {
	  g_free (puk);
	  break;
	}
        result = sim_send_puk_code (&error, &current_status, puk, pin);
        g_free (pin);
        g_free (puk);
      break;
    }
    
  }

  /* Now we can close the window */
  gtk_widget_destroy (data.dialog);

  /* Unset the beacon here to state that the GUI is no longer active */
  is_sim_code_gui_active = FALSE;
}

void
display_pin_error (const char *message)
{
  GtkWidget *dlg;
  
  dlg = gtk_message_dialog_new (NULL, 0, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                message);
  gtk_dialog_run (GTK_DIALOG (dlg));
  gtk_widget_destroy (dlg);
}

