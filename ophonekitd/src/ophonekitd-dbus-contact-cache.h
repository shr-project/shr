/*
 *  Copyright (C) 2009
 *      Authors (alphabetical) :
 *              Klaus 'mrmoku' Kurzmann (mok@fluxnetz.de)
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


#ifndef _OPHONEKITD_DBUS_CONTACT_CACHE_H
#define _OPHONEKITD_DBUS_CONTACT_CACHE_H

#include <glib-object.h>

#define OPHONEKITD_TYPE_CONTACT_CACHE_SERVICE      (ophonekitd_contact_cache_service_get_type ())
#define OPHONEKITD_CONTACT_CACHE_SERVICE(object)   (G_TYPE_CHECK_INSTANCE_CAST ((object), OPHONEKITD_TYPE_CONTACT_CACHE_SERVICE, OphonekitdContactCacheService))
#define OPHONEKITD_CONTACT_CACHE_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), OPHONEKITD_TYPE_CONTACT_CACHE_SERVICE, OphonekitdContactCacheServiceClass))
#define OPHONEKITD_IS_CONTACT_CACHE_SERVICE(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), OPHONEKITD_TYPE_CONTACT_CACHE_SERVICE))
#define OPHONEKITD_IS_CONTACT_CACHE_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), OPHONEKITD_TYPE_CONTACT_CACHE_SERVICE))
#define OPHONEKITD_CONTACT_CACHE_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), OPHONEKITD_TYPE_CONTACT_CACHE_SERVICE, OphonekitdContactCacheServiceClass))

typedef struct _OphonekitdContactCacheService OphonekitdContactCacheService;
typedef struct _OphonekitdContactCacheServiceClass OphonekitdContactCacheServiceClass;

GType ophonekitd_contact_cache_service_get_type (void);

struct _OphonekitdContactCacheService {
    GObject parent;
};

struct _OphonekitdContactCacheServiceClass {
    GObjectClass parent;
};


void ophonekitd_contact_cache_load(void);
void ophonekitd_contact_cache_unload(void);
void ophonekitd_contact_cache_service_lookup (OphonekitdContactCacheService *object, const char *number, DBusGMethodInvocation *context);

OphonekitdContactCacheService *ophonekitd_contact_cache_service_new (void);

#endif

