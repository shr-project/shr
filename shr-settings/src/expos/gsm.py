#!/usr/bin/python

#
# Copyright (C) 2008 by OpenMoko, Inc.
# Written by Willie Chen <willie_chen@openmoko.com>
#            Marek Lindner <marek@openmoko.com>
# All Rights Reserved
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

import os, dbus


def debug(msg):
    print msg

class gsm:
        def __init__(self, dbus, system_bus, name="GSM"):
                self.name = name
                self.default_status = "unknown"
                self.runnable = 1

                self.status = -1
                self.status_str = self.default_status
                self.dbus = dbus
                self.system_bus = system_bus
                self.dbus_name = "org.shr.ophonekitd.Usage"
                self.dbus_path = "/org/shr/ophonekitd/Usage"
                self.dbus_iface = "org.shr.ophonekitd.Usage"
                debug("enter init device : " + self.name)

        def dbus_get_resource_state(self, state):
                self.status = 1 if state else 0
                self.update_label()

        def dbus_set_state(self):
                debug("state changed")

        def dbus_error_handler(self, error):
                print "error getting bus names - %s" % str(error)
                self.status = -1
                self.status_str = self.default_status
                self.runnable = 0
                self.update_label()

        def dbus_owner_changed(self, owner):
                debug("dbus_owner_changed: " + str(owner))

                if not self.runnable and owner <> "":
                        self.connect_to_ophonekitd()

                        if self.runnable:
                                self.get_status()

        def after_animate_click(self, edje_obj, emission, source):
                debug('onclick')
                if (self.runnable):
                        self.status = 0 if self.status == 1 else 1
                        self.set_state()
                        self.update_label()

        def init_status(self):
                #debug('init_status')
                self.connect_to_ophonekitd()

                if self.runnable:
                        self.get_state()
                else:
                        self.system_bus.watch_name_owner(self.dbus_name, self.dbus_owner_changed)
                        self.update_label()

        def connect_to_ophonekitd(self):
                self.runnable = 1
                try:
                        self.ophonekitd_obj = self.system_bus.get_object(self.dbus_name, self.dbus_path, introspect=False)
                except:
                        debug("Can't get ophonekitd object:")
                        debug(sys.exc_info()[1])
                        self.runnable = 0
                else:
                        try:
                                self.ophonekitd_iface = self.dbus.Interface(self.ophonekitd_obj, dbus_interface=self.dbus_iface)
                        except:
                                debug("Can't get ophonekitd interface:")
                                debug(sys.exc_info()[1])
                                self.runnable = 0
                        else:
                                self.ophonekitd_iface.GetResourceState("GSM", reply_handler=self.dbus_get_resource_state, error_handler=self.dbus_error_handler)
        def get_state(self):
                self.ophonekitd_iface.GetResourceState("GSM",reply_handler=self.dbus_get_resource_state, error_handler=self.dbus_error_handler)

        def set_state(self):
                debug('set_state: ' + str(self.status))
                if self.status == 1:
                        self.ophonekitd_iface.RequestResource("GSM",reply_handler=self.dbus_set_state, error_handler=self.dbus_error_handler)
                else:
                        self.ophonekitd_iface.ReleaseResource("GSM",reply_handler=self.dbus_set_state, error_handler=self.dbus_error_handler)

        def update_label(self):
                debug('update_label: ' + str(self.status))
                if self.runnable:
                        self.status_str = "on" if self.status == 1 else "off"
                        debug('update_label: ' + self.status_str)
                self.edje_obj.part_text_set("item-status", self.status_str)

