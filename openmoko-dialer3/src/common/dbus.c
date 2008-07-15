#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include "dbus.h"

static void lose (const char *str, ...)
{
	va_list args;

	va_start (args, str);

	vfprintf (stderr, str, args);
	fputc ('\n', stderr);

	va_end (args);

	exit (1);
}

static void lose_gerror (const char *prefix, GError *error)
{
	lose ("%s: %s", prefix, error->message);
}

static void network_status_handler (DBusGProxy *proxy, const  GHashTable ** status, gpointer user_data)
{
  printf ("Received network status");
}

static void network_signal_strength_handler (DBusGProxy *proxy, const int signal_strength, gpointer user_data)
{
  printf ("Received network signal strength");
}

static void sim_auth_status_handler (DBusGProxy *proxy, const char *status, gpointer user_data)
{
  printf ("Received sim auth status");
}

static void call_status_handler (DBusGProxy *proxy, const int id, const char *status, const GHashTable ** properties, gpointer user_data)
{
  printf ("Received call status");
}



int main(int argc, char ** argv) {
	DBusGConnection *bus;
	DBusGProxy *networkBus, *simBus, *callBus;
	GError *error = NULL;
	GMainLoop *mainloop = NULL;
	g_type_init ();

	{
		GLogLevelFlags fatal_mask;

		fatal_mask = g_log_set_always_fatal (G_LOG_FATAL_MASK);
		fatal_mask |= G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL;
		g_log_set_always_fatal (fatal_mask);
	}

	bus = dbus_g_bus_get (DBUS_BUS_SYSTEM, &error);


	if (!bus)
		lose_gerror ("Couldn't connect to system bus", error);

	networkBus = dbus_g_proxy_new_for_name (bus, NETWORKD_BUS, BUS_PATH, NETWORK_INTERFACE);
	simBus = dbus_g_proxy_new_for_name (bus, GSMD_BUS, BUS_PATH, SIM_INTERFACE);
	callBus = dbus_g_proxy_new_for_name (bus, GSMD_BUS, BUS_PATH, CALL_INTERFACE);

  	mainloop = g_main_loop_new (NULL, FALSE);
	

	dbus_g_proxy_add_signal (networkBus, "Status", DBUS_TYPE_G_STRING_VARIANT_HASHTABLE, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (networkBus, "Status", G_CALLBACK (network_status_handler),
                                     NULL, NULL);

	g_main_loop_run (mainloop);

	exit(EXIT_SUCCESS);

}
