/*  openmoko-panel-gsm.c
 *
 *  Authored by Michael 'Mickey' Lauer <mlauer@vanille-media.de>
 *  Adapted and modified for SHR/Frameworkd by Julien Cassignol <ainulindale@gmail.com>
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
 *
 *  Current Version: $Rev$ ($Date$) [$Author: mickey $]
 */
#include <libmokopanelui2/moko-panel-applet.h>

#include <libnotify/notify.h>

#include <gtk/gtkimage.h>
#include <gtk/gtkbox.h>
#include <gtk/gtk.h>

#include <string.h>
#include <time.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <frameworkd-glib/frameworkd-glib-dbus.h>
#include <frameworkd-glib/frameworkd-glib-network.h>
#include <frameworkd-glib/frameworkd-glib-device.h>

#define GSM_PWOERON_FILENAME "/sys/bus/platform/devices/neo1973-pm-gsm.0/power_on"
#define QUERY_FREQ 5
/* Just change this is gsmd changes */
#define _MAX_SIGNAL 30.0

typedef struct {
    MokoPanelApplet* mokoapplet;
    gboolean gprs_mode;
    int strength;
    int type;
    char* lac;
    char* cell;
    char operator_name[255];

    GtkMenuItem* information;
    guint timeout_id;
    int state;
} GsmApplet;

static GsmApplet* theApplet = NULL;

/* forwards */
static void
gsm_applet_show_status(GtkWidget* menu, GsmApplet* applet);
static void
gsm_applet_update_signal_strength(int, GsmApplet* );
static void
gsm_applet_power_up_antenna(GtkWidget* menu, GsmApplet* applet);

void connect_to_frameworkd();
void gsmpanel_network_signal_strength_handler (const int signal_strength);
void gsmpanel_network_status_handler (GHashTable * status);
/* internal */
static void
gsm_applet_free(GsmApplet *applet)
{
    g_slice_free( GsmApplet, applet );
}

static void
gsm_applet_gsmd_connection_status(gboolean status)
{
    g_debug( "gsm_applet_gsmd_connection_status: status = %s", status ? "TRUE" : "FALSE" );
    if ( status )
    {
    
    }
    else
    {
        strcpy( theApplet->operator_name, "<unknown>" );
        gsm_applet_update_signal_strength(99, theApplet );
        gsm_applet_show_status(NULL, theApplet);
    }
}

static void
gsm_applet_update_signal_strength(int strength,  GsmApplet* applet)
{
    gint pixmap = 0;
    gchar *image = NULL;
    applet->strength = strength;

    g_debug( "gsm_applet_update_signal_strength: signal strength = %d",
              strength );

    if ( strength == 99 )
    {
        moko_panel_applet_set_icon( applet->mokoapplet, PKGDATADIR "/SignalStrength_NR.png" );
        return;
    }

    gfloat percent = (strength / _MAX_SIGNAL) * 100;

    if ( percent == 0 )
      pixmap = 0;
    else if ( percent < 20 )
      pixmap = 1;
    else if ( percent < 40 )
      pixmap = 2;
    else if ( percent < 60 )
      pixmap = 3;
    else if ( percent < 80 )
      pixmap = 4;
    else
      pixmap = 5;

    image = g_strdup_printf( "%s/SignalStrength%s%02d.png",
                             PKGDATADIR,
                             applet->gprs_mode ? "25g_" : "_", pixmap );

    moko_panel_applet_set_icon( applet->mokoapplet, image );

    g_free( image );
}

static void
gsm_applet_gsm_antenna_status(gboolean status)
{
    g_debug( "gsm_applet_gsm_antenna_status: status = %s", status ? "ON" : "OFF" );
    if(status) {
	    theApplet->type = 6;
	    gsm_applet_update_signal_strength( theApplet->strength, theApplet );
	    gsm_applet_show_status( 0, theApplet );
    }
    else {
	    /* notify user antenna is OFF */
	    theApplet->type = 7;
	    gsm_applet_update_signal_strength( 99, theApplet );
	    gsm_applet_show_status( 0, theApplet );
    }

   
}

static void gsm_applet_network_current_operator_cb( const gchar* name)
{
    if ( strcmp( name, theApplet->operator_name ) != 0 )
    {
        strcpy( theApplet->operator_name, name );
        gsm_applet_update_signal_strength( theApplet->strength, theApplet );
        gsm_applet_show_status( 0, theApplet );
    }
}

static void
gsm_applet_network_registration_cb(int type,
                                  char* lac,
                                  char* cell)
{
    g_debug( "gsm_applet_network_registration_cb: updating netreg values" );
    theApplet->type = type;
    theApplet->lac = lac;
    theApplet->cell = cell;

}

static void
gsm_applet_show_status(GtkWidget* menu, GsmApplet* applet)
{
    const gchar* summary = 0;
    const gchar* details = 0;
    switch ( applet->type )
    {
        case 0:
            summary = g_strdup( "Not searching" );
        break;

        case 1:
            summary = g_strdup_printf( "Connected to '%s'", applet->operator_name );
            details = g_strdup_printf( "Type: Home Network\nCell ID: %s : %s\nSignal: %i dbM", applet->lac, applet->cell, -113 + applet->strength*2 );
        break;

        case 2: summary = g_strdup( "Searching for Service" );
        break;

        case 3: summary = g_strdup( "Registration Denied" );
        break;

        case 5:
            summary = g_strdup_printf( "Connected to '%s'", applet->operator_name );
            details = g_strdup_printf( "Type: Roaming\nCell ID: %s : %s\nSignal: %i dbM", applet->lac, applet->cell, -113 + applet->strength*2 );
        break;
	
        case 6: summary = g_strdup( "GSM Antenna Power-Up" );
        break;
        
	case 7: summary = g_strdup( "GSM Antenna Power-Down" );
        break;
	
	case 8: summary = g_strdup( "GSM Modem Power-Down" );
        break;

	case 9: summary = g_strdup( "GSM Modem Power-Up" );
        break;

	default:
            summary = g_strdup( "Unknown" );
    }

    notify_notification_show( notify_notification_new( summary, details, NULL, NULL ), NULL );
}

static void
gsm_applet_power_up_antenna(GtkWidget* menu, GsmApplet* applet)
{
    device_set_antenna_power(TRUE, NULL);
    gsm_applet_gsm_antenna_status(TRUE);
}

static void
gsm_applet_autoregister_network(GtkWidget* menu, GsmApplet* applet)
{
    network_register(NULL);
}

static void
gsm_applet_power_down_antenna(GtkWidget* menu, GsmApplet* applet)
{
    device_set_antenna_power(FALSE, NULL);
    gsm_applet_gsm_antenna_status(FALSE);
}


static int 
gsm_applet_power_get() 
{
    char buf[64];
    FILE * f = fopen(GSM_PWOERON_FILENAME, "r");
    int ret;
    if (!f) {
	    printf("Open file %s failed!!\n",GSM_PWOERON_FILENAME);
	    return 0;
    }
    ret = fread(buf,sizeof(char),sizeof(buf)/sizeof(char),f);
    fclose(f);
    if (ret > 0 && buf[0]=='1') {
	    return 1;
    }
    return 0;
}

static void
gsm_applet_update_visibility (GsmApplet *applet)
{
    if(applet->state == gsm_applet_power_get())
	    return;

    if (!gsm_applet_power_get()) {
	    theApplet->type = 8;
	    gsm_applet_update_signal_strength( 99, applet );
	    gsm_applet_show_status( 0, applet );
	    applet->state = 0;
    } else {
	    theApplet->type = 9;
	    gsm_applet_update_signal_strength( 0, applet );
	    gsm_applet_show_status( 0, applet );
	    applet->state = 1;
    }
}


static gboolean
gsm_applet_timeout_cb (gpointer data)
{
  gsm_applet_update_visibility ((GsmApplet *)data);

  return TRUE;
}

G_MODULE_EXPORT GtkWidget*
mb_panel_applet_create(const char* id, GtkOrientation orientation)
{
    GsmApplet* applet = g_slice_new0(GsmApplet);
    theApplet = applet; // nasty global variable
    strcpy( applet->operator_name, "<unknown>" );
    MokoPanelApplet* mokoapplet = applet->mokoapplet = MOKO_PANEL_APPLET(moko_panel_applet_new());

    notify_init ("GSM Applet");

    moko_panel_applet_set_icon( mokoapplet, PKGDATADIR "/SignalStrength_NR.png" );

    applet->gprs_mode = FALSE;
    gtk_widget_show_all( GTK_WIDGET(mokoapplet) );

    applet->state = 1;

    connect_to_frameworkd();

    // tap-with-hold menu (NOTE: temporary: left button atm.)
    GtkMenu* menu = GTK_MENU (gtk_menu_new());

    GtkWidget* title = gtk_frame_new( "GSM Network" );
    gtk_frame_set_label_align( GTK_FRAME(title), 0.5, 0.5 );
    gtk_frame_set_shadow_type( GTK_FRAME(title), GTK_SHADOW_IN );
    gtk_widget_set_name( title, "GsmAppletMenu" );

    GtkWidget* titleitem = gtk_menu_item_new();
    g_signal_connect( G_OBJECT(titleitem), "activate", G_CALLBACK(gsm_applet_show_status), applet );
    gtk_menu_shell_append( GTK_MENU_SHELL(menu), titleitem );
    gtk_container_add( GTK_CONTAINER(titleitem), title );

    GtkWidget* item1 = gtk_menu_item_new_with_label( "Power-Up GSM Antenna" );
    g_signal_connect( G_OBJECT(item1), "activate", G_CALLBACK(gsm_applet_power_up_antenna), applet );
    gtk_menu_shell_append( GTK_MENU_SHELL(menu), item1 );
    GtkWidget* item2 = gtk_menu_item_new_with_label( "Autoregister with Network" );
    g_signal_connect( G_OBJECT(item2), "activate", G_CALLBACK(gsm_applet_autoregister_network), applet );
    gtk_menu_shell_append( GTK_MENU_SHELL(menu), item2 );
    GtkWidget* item3 = gtk_menu_item_new_with_label( "Power-Down GSM Antenna" );
    g_signal_connect( G_OBJECT(item3), "activate", G_CALLBACK(gsm_applet_power_down_antenna), applet );
    gtk_menu_shell_append( GTK_MENU_SHELL(menu), item3 );
    
    gtk_widget_show_all( GTK_WIDGET(menu) );
    moko_panel_applet_set_popup( mokoapplet, GTK_WIDGET (menu), MOKO_PANEL_APPLET_CLICK_POPUP );

    applet->timeout_id = g_timeout_add_seconds (QUERY_FREQ, gsm_applet_timeout_cb, 
      applet);
      
    return GTK_WIDGET(mokoapplet);
}

void connect_to_frameworkd() {
        FrameworkdHandlers fwHandler;
        fwHandler.networkStatus = gsmpanel_network_status_handler;
        fwHandler.simAuthStatus = NULL;
        fwHandler.callCallStatus = NULL;
        fwHandler.smsMessageSent = NULL;
        fwHandler.smsIncomingMessage = NULL;
        fwHandler.networkSignalStrength = gsmpanel_network_signal_strength_handler;
        dbus_connect_to_bus(&fwHandler);
}

void gsmpanel_network_status_handler (GHashTable * status)
{
    char *provider = NULL;
    char *lac = NULL;
    char *cell = NULL;
    
    int regStatus = network_get_registration_status(status); 
    int type = -1;
    
    lac = network_get_location_area(status);
    cell = network_get_cell_id(status);
    provider = network_get_provider(status);
    switch(regStatus) {
        case NETWORK_PROPERTY_REGISTRATION_UNREGISTERED:
            type = 0;
            break;
        case NETWORK_PROPERTY_REGISTRATION_HOME:
            type = 1;
            gsm_applet_network_current_operator_cb(provider);        
            break;
        case NETWORK_PROPERTY_REGISTRATION_BUSY:
            type = 2;
            break;
        case NETWORK_PROPERTY_REGISTRATION_DENIED:
            type = 3;
            break;
        case NETWORK_PROPERTY_REGISTRATION_ROAMING:
            type = 5;
            gsm_applet_network_current_operator_cb(provider);
            break;
    }
    gsm_applet_network_registration_cb(type, lac, cell);
   
}

void gsmpanel_network_signal_strength_handler (const int signal_strength)
{
    gsm_applet_update_signal_strength(signal_strength, theApplet);
}


