
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <frameworkd-phonegui/frameworkd-phonegui.h>
#include "phoneuid-call-management.h"
#include "phoneuid-dialer.h"
#include "phoneuid-dialogs.h"
#include "phoneuid-contacts.h"
#include "phoneuid-messages.h"
#include "phoneuid-dbus-common.h"
#include "phoneuid-contacts-service-glue.h"
#include "phoneuid-messages-service-glue.h"
#include "phoneuid-dialogs-service-glue.h"
#include "phoneuid-dialer-service-glue.h"
#include "phoneuid-call-management-service-glue.h"

static int logfile = -1;

static void
_log_handler(const gchar *domain, GLogLevelFlags level, const gchar *message,
		gpointer userdata)
{
	struct timeval tv;
	struct tm ptime;
	gettimeofday(&tv, NULL);
        localtime_r(&tv.tv_sec, &ptime);

	char *msg = g_strdup_printf("%04d.%02d.%02d %02d:%02d:%02d.%06d %s\n",
			ptime.tm_year+1900, ptime.tm_mon, ptime.tm_mday,
			ptime.tm_hour, ptime.tm_min, ptime.tm_sec, tv.tv_usec,
			message);
	write(logfile, msg, strlen(msg));
	free(msg);
}

static gpointer
dbus_register_object(DBusGConnection * connection,
		     DBusGProxy * proxy,
		     GType object_type,
		     const DBusGObjectInfo * info, const gchar * path)
{
	GObject *object = g_object_new(object_type, NULL);
	dbus_g_object_type_install_info(object_type, info);
	dbus_g_connection_register_g_object(connection, path, object);
	return object;
}

static void
phoneuid_dbus_setup()
{
	guint result;
	GError *error = NULL;
	DBusGConnection *connection;
	DBusGProxy *proxy;

	connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (error) {
		g_error("%d: %s", error->code, error->message);
	}
	proxy = dbus_g_proxy_new_for_name(connection,
			DBUS_SERVICE_DBUS,
			DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);

	/* -- request org.shr.phoneuid -- */
	g_debug("requesting name %s", PHONEUID_SERVICE);
	if (!org_freedesktop_DBus_request_name(proxy, PHONEUID_SERVICE,
				DBUS_NAME_FLAG_DO_NOT_QUEUE,
				&result, &error)) {
		g_error("Error requesting name: %d %s",
				error->code, error->message);
	}
	if (result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
		g_error("Got result code %u from requesting %s", result,
				PHONEUID_SERVICE);
	}

	/* -- register org.shr.phoneuid.CallManagement -- */
	g_debug("registering %s", PHONEUID_CALL_MANAGEMENT_PATH);
	dbus_register_object(connection, proxy,
			PHONEUID_TYPE_CALL_MANAGEMENT_SERVICE,
			&dbus_glib_phoneuid_call_management_service_object_info,
			PHONEUID_CALL_MANAGEMENT_PATH);

	/* -- register org.shr.phoneuid.Dialer -- */
	g_debug("registering %s", PHONEUID_DIALER_PATH);
	dbus_register_object(connection, proxy,
			PHONEUID_TYPE_DIALER_SERVICE,
			&dbus_glib_phoneuid_dialer_service_object_info,
			PHONEUID_DIALER_PATH);

	/* -- register org.shr.phoneuid.Dialogs -- */
	g_debug("registering %s", PHONEUID_DIALOGS_PATH);
	dbus_register_object(connection, proxy,
			PHONEUID_TYPE_DIALOGS_SERVICE,
			&dbus_glib_phoneuid_dialogs_service_object_info,
			PHONEUID_DIALOGS_PATH);

	/* -- register org.shr.phoneuid.Contacts -- */
	g_debug("registering %s", PHONEUID_CONTACTS_PATH);
	dbus_register_object(connection, proxy,
			PHONEUID_TYPE_CONTACTS_SERVICE,
			&dbus_glib_phoneuid_contacts_service_object_info,
			PHONEUID_CONTACTS_PATH);

	/* -- register org.shr.phoneuid.Messages -- */
	g_debug("registering %s", PHONEUID_MESSAGES_PATH);
	dbus_register_object(connection, proxy,
			PHONEUID_TYPE_MESSAGES_SERVICE,
			&dbus_glib_phoneuid_messages_service_object_info,
			PHONEUID_MESSAGES_PATH);

}


int
main(int argc, char **argv)
{
	if (argc == 1) {
		/* --- daemonize --- */
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		umask(0077);

		if (fork())
			return (0);

		chdir("/");
		setsid();
		setpgrp();
	}

	logfile = open("/tmp/phoneuid.log", O_WRONLY | O_CREAT | O_APPEND);
	if (logfile == -1) {
		printf("Error creating the logfile!!!");
		return (-3);
	}
	g_log_set_default_handler(_log_handler, NULL);
	g_type_init();

	if (!g_thread_supported())
		g_thread_init(NULL);
	dbus_g_thread_init();

	phonegui_load("phoneuid");
	phonegui_init(argc, argv, NULL);
	phoneuid_dbus_setup();
	phonegui_loop();
	g_debug("exited from phonegui_loop!");
}


