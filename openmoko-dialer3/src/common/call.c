#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include "call.h"

static void call_status_handler (DBusGProxy *proxy, const int id, const char *status, const GHashTable ** properties, gpointer user_data)
{
  printf ("Received call status");
}

