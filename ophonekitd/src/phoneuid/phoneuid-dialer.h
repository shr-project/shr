/*
 *  Copyright (C) 2009
 *      Authors (alphabetical) :
 *              Klaus 'mrmoku' Kurzmann <mok@fluxnetz.de>
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

#ifndef _PHONEUID_DIALER_H
#define _PHONEUID_DIALER_H

#define PHONEUID_TYPE_DIALER_SERVICE            (phoneuid_dialer_service_get_type ())
#define PHONEUID_DIALER_SERVICE(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), PHONEUID_TYPE_DIALER_SERVICE, PhoneuidDialerService))
#define PHONEUID_DIALER_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PHONEUID_TYPE_DIALER_SERVICE, PhoneuidDialerServiceClass))
#define PHONEUID_IS_DIALER_SERVICE(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), PHONEUID_TYPE_DIALER_SERVICE))
#define PHONEUID_IS_DIALER_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PHONEUID_TYPE_DIALER_SERVICE))
#define PHONEUID_DIALER_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), PHONEUID_TYPE_DIALER_SERVICE, PhoneuidDialerServiceClass))


typedef struct _PhoneuidDialerService PhoneuidDialerService;
typedef struct _PhoneuidDialerServiceClass PhoneuidDialerServiceClass;

GType phoneuid_dialer_service_get_type(void);

struct _PhoneuidDialerService {
	GObject parent;
};

struct _PhoneuidDialerServiceClass {
	GObjectClass parent;
	DBusGConnection *connection;
};

gboolean
phoneuid_dialer_service_display(PhoneuidDialerService *object,
		GHashTable *options, DBusGMethodInvocation *context);

PhoneuidDialerService *phoneuid_dialer_service_new(void);



#endif

