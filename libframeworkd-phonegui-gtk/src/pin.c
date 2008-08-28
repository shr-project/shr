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

#include "dialer-textview.h"
#include "dialer-panel.h"
#include <frameworkd-glib/frameworkd-glib-sim.h>
#include <frameworkd-glib/frameworkd-glib-dbus.h>
#include "pin.h"
#include "frameworkd-phonegui.h"

static gboolean is_sim_code_gui_active = FALSE;
static int code_to_ask = 0;
static MokoPinData staticData;

        static void
on_pad_user_input (MokoDialerPanel *panel, const gchar digit, MokoPinData *data)
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
                data->code[strlen (data->code)-1] = '\0';
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

void phonegui_display_pin_UI (const int initial_status) {
        code_to_ask = initial_status;

        /* First, let's check if we have anything to do here */
        if (is_sim_code_gui_active)
        {
                if (code_to_ask == SIM_READY)
                {
                        /* We shouldn't be there... doesn't happen 
                           or should destroy_pin_UI not be called from sim.c ? */
                }
                return;
        }

        GtkWidget *pad;

        /* Set a beacon around here to state that the GUI is active */
        is_sim_code_gui_active = TRUE;

        /* Build the GUI */
        staticData.dialog = gtk_dialog_new_with_buttons ("Enter PIN code", NULL, 0,
                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                        GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
        gtk_dialog_set_has_separator (GTK_DIALOG (staticData.dialog), FALSE);

        staticData.display = moko_dialer_textview_new ();
        gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (staticData.dialog)->vbox), staticData.display);

        pad = moko_dialer_panel_new ();
        gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (staticData.dialog)->vbox), pad);
        g_signal_connect (pad, "user_input", G_CALLBACK (on_pad_user_input), &staticData);

        gtk_widget_show_all (GTK_DIALOG (staticData.dialog)->vbox);

        /* The PIN/PUK conversation */
        display_pin_puk_dialog (code_to_ask);
}

void display_pin_puk_dialog() {
        staticData.code = NULL;

        switch (code_to_ask)
        {
                case SIM_PIN_REQUIRED:
                case SIM_PIN2_REQUIRED:
                        display_pin_dialog();
                        break;
                case SIM_PUK_REQUIRED:
                case SIM_PUK2_REQUIRED:
                        display_puk_dialog();
                        break;

        }

}

        void
display_pin_error (const char *message)
{
        GtkWidget *dlg;

        dlg = gtk_message_dialog_new (NULL, 0, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, message);
        gtk_dialog_run (GTK_DIALOG (dlg));
        gtk_widget_destroy (dlg);
}

        void display_pin_dialog() {
                if (gtk_dialog_run (GTK_DIALOG (staticData.dialog)) == GTK_RESPONSE_OK)
                {
                        set_code_ui_state_busy ();
                        sim_send_auth_code (staticData.code, sim_pin_puk_callback, NULL);
                }
        }

void display_puk_dialog() {
        char *puk = NULL, *pin = NULL;

        /* I need to set a new title to the dialog here */
        if (gtk_dialog_run (GTK_DIALOG (staticData.dialog)) == GTK_RESPONSE_OK)
        {
                puk = g_strdup (staticData.code);
        } else {
                return;
        }
        /* I need to set a new title to the dialog here */
        if (gtk_dialog_run (GTK_DIALOG (staticData.dialog)) == GTK_RESPONSE_OK)
        {
                pin = g_strdup (staticData.code);
        } else {
                g_free (puk);
                return;
        }
        set_code_ui_state_busy ();
        sim_unlock (puk, pin, sim_pin_puk_callback, NULL);
        
        g_free (pin);
        g_free (puk);
}

        void
sim_pin_puk_callback (GError *error, gpointer userdata)
{
        if(error != NULL) {
                if(IS_SIM_ERROR(error, SIM_ERROR_BLOCKED)) {
                        /* display_pin_error("") */                    
                } else if (IS_SIM_ERROR(error, SIM_ERROR_AUTH_FAILED)) {
                        /* display_pin_error("") */
                }
                display_pin_puk_dialog();
        } else {
                /* We should have SIM_READY then UI should be destroyed. */
        }
}

        void
phonegui_destroy_pin_UI ()
{
        /* Now we can close the window */
        gtk_widget_destroy (staticData.dialog);

        /* Unset the beacon here to state that the GUI is no longer active */
        is_sim_code_gui_active = FALSE;        
}

        void
set_code_ui_state_busy ()
{
#ifdef DEBUG
        printf("Setting UI state to BUSY\n");
#endif
        /* TODO */
}

        void
set_code_ui_state_ready ()
{
#ifdef DEBUG
        printf("Setting UI state to READY.\n");
#endif
        /* TODO */
}
