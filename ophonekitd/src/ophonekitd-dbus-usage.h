/*
 *  Copyright (C) 2008
 *      Authors (alphabetical) :
 *              Marc-Olivier Barre <marco@marcochapeau.org>
 *              Julien Cassignol <ainulindale@gmail.com>
 *              quickdev
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Public License as published by
 *  the Free Software Foundation; version 2 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 */

#ifndef _OPHONEKITD_DBUS_USAGE_H
#define _OPHONEKITD_DBUS_USAGE_H

#include <glib-object.h>

#define OPHONEKITD_TYPE_USAGE_SERVICE            (ophonekitd_usage_service_get_type ())
#define OPHONEKITD_USAGE_SERVICE(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), OPHONEKITD_TYPE_USAGE_SERVICE, OphonekitdUsageService))
#define OPHONEKITD_USAGE_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), OPHONEKITD_TYPE_USAGE_SERVICE, OphonekitdUsageServiceClass))
#define OPHONEKITD_IS_USAGE_SERVICE(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), OPHONEKITD_TYPE_USAGE_SERVICE))
#define OPHONEKITD_IS_USAGE_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), OPHONEKITD_TYPE_USAGE_SERVICE))
#define OPHONEKITD_USAGE_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), OPHONEKITD_TYPE_USAGE_SERVICE, OphonekitdUsageServiceClass))


typedef struct _OphonekitdUsageService OphonekitdUsageService;
typedef struct _OphonekitdUsageServiceClass OphonekitdUsageServiceClass;

GType ophonekitd_usage_service_get_type(void);

struct _OphonekitdUsageService {
	GObject parent;
};

struct _OphonekitdUsageServiceClass {
	GObjectClass parent;
};

void ophonekitd_usage_service_get_resource_state(OphonekitdUsageService *
						 object, const char *resource,
						 DBusGMethodInvocation *
						 context);
void ophonekitd_usage_service_release_resource(OphonekitdUsageService * object,
					       const char *resource,
					       DBusGMethodInvocation * context);
void ophonekitd_usage_service_request_resource(OphonekitdUsageService * object,
					       const char *resource,
					       DBusGMethodInvocation * context);

OphonekitdUsageService *ophonekitd_usage_service_new(void);

#endif
