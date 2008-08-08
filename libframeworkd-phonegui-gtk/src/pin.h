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

#ifndef MOKO_PIN_H
#define MOKO_PIN_H

typedef struct
{
        GtkWidget *display;
        GtkWidget *dialog;
        gchar *code;
} MokoPinData;


static void
on_pad_user_input (MokoDialerPanel *panel, const gchar digit,
                MokoPinData *data);
void display_pin_puk_dialog();
void display_pin_error (const char *message);
void display_pin_dialog();
void display_puk_dialog();
void sim_pin_puk_callback(GError *error, gpointer userdata);
void set_code_ui_state_busy ();
void set_code_ui_state_ready ();

#endif
