/*
 *  Copyright (C) 2007 Openmoko, Inc.
 *
 *  Authored by:
 *    Marc-Olivier Barre <marco@marcochapeau.org>
 *    OpenedHand Ltd <info@openedhand.com>
 *    quickdev
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

#include "dialer-main.h"
#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus-glib.h>
#include <frameworkd-glib/frameworkd-glib-dbus.h>
#include "dialer-phonegui.h"


int main(int argc, char **argv) {
    /* Connect to frameworkd */
    connect_to_frameworkd();

    /* Load, connect and initiate phonegui */
    phonegui_load("openmoko-dialer3");
    phonegui_connect();
    phonegui_dialer_launch();

    return EXIT_SUCCESS;
}

void connect_to_frameworkd() {
    FrameworkdHandlers fwHandler;
    fwHandler.networkStatus = NULL;
    fwHandler.networkSignalStrength = NULL;
    fwHandler.simAuthStatus = NULL;
    fwHandler.simIncomingStoredMessage = NULL;
    fwHandler.callCallStatus = NULL;
    dbus_connect_to_bus(&fwHandler);
}

