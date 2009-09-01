#include "call-dbus.h"

DBusGConnection* init_g_connection()
{
	DBusGConnection* bus;
	GError* error = NULL;
	
	g_debug("get DBus Connection!\n");
	bus = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
	if (bus == NULL) {	 
		g_debug ("Error while connecting to the bus :%s\n", error->message);
		g_error_free(error);
		return NULL;
	 }	
	return bus;	
}

int init_proxy(DBusGConnection* bus, 
               DBusGProxy** proxy,
               char*  bus_name, 
               char* object_path, 
               char* iface)
{
	*proxy = dbus_g_proxy_new_for_name(bus, bus_name, object_path, iface);
	if (proxy == NULL) {
		g_debug ("The Device Proxy for %s couldn't be found\n", object_path);
		return 1;
	}
	g_debug("proxy initialized!\n");
	return 0;
}
