#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include "network.h"
#include "dbus.h"

static void network_status_handler (DBusGProxy *proxy, const  GHashTable ** status, gpointer user_data)
{
	printf ("Received network status");
}

static void network_signal_strength_handler (DBusGProxy *proxy, const int signal_strength, gpointer user_data)
{
	printf ("Received network signal strength");
}

