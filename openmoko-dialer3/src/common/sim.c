#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include "sim.h"
#include "dbus.h"

static void sim_auth_status_handler (DBusGProxy *proxy, const char *status, gpointer user_data)
{ 
  printf ("Received sim auth status");
  }


int get_authentication_state() {

	char *status = NULL;
	GError *error = NULL;
	int result = -1;
	if (!dbus_g_proxy_call (simBus, "GetAuthStatus", &error,
				G_TYPE_INVALID,
				G_TYPE_STRING, &status, G_TYPE_INVALID))
		lose_gerror ("Failed to complete GetAuthStatus", error);
	
	if(strcmp(status,DBUS_SIM_READY))
		result = SIM_READY;
	else if(strcmp(status,DBUS_SIM_PIN_REQUIRED))
		result = SIM_PIN_REQUIRED;
	else if(strcmp(status,DBUS_SIM_PUK_REQUIRED))
		result = SIM_PUK_REQUIRED;
	else if(strcmp(status,DBUS_SIM_PIN2_REQUIRED))
		result = SIM_PIN2_REQUIRED;
	else
		result = SIM_PUK2_REQUIRED;
	free(status);
	return result;
}


