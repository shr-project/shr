

#ifndef _DBUS_H
#define _DBUS_H

#define NETWORKD_BUS		"org.freesmartphone.onetworkd"
#define GSMD_BUS		"org.freesmartphone.ogsmd"
#define BUS_PATH		"/org/freesmartphone/GSM/Device"
#define CALL_INTERFACE		"org.freesmartphone.GSM.Call"
#define SIM_INTERFACE		"org.freesmartphone.GSM.SIM"
#define NETWORK_INTERFACE 	"org.freesmartphone.GSM.Network"
#define	DEVICE_INTERFACE	"org.freesmartphone.GSM.Device"

#define DBUS_TYPE_G_STRING_VARIANT_HASHTABLE (dbus_g_type_get_map ("GHashTable", G_TYPE_STRING, G_TYPE_VALUE))

static DBusGProxy *networkBus = NULL, *simBus = NULL, *callBus = NULL;
static void network_status_handler (DBusGProxy *proxy, const  GHashTable ** status, gpointer user_data);
static void network_signal_strength_handler (DBusGProxy *proxy, const int signal_strength, gpointer user_data);
static void sim_auth_status_handler (DBusGProxy *proxy, const char *status, gpointer user_data);
static void call_status_handler (DBusGProxy *proxy, const int id, const char *status, const GHashTable ** properties, gpointer user_data);
static void lose (const char *str, ...);
static void lose_gerror (const char *prefix, GError *error);

#endif
