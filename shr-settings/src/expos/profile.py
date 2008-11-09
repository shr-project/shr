#!/usr/bin/python

#
# Copyright (C) 2008 by OpenMoko, Inc.
# Written by Marek Lindner <marek@openmoko.com>
# All Rights Reserved
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

import sys


def debug(msg):
	print msg

class profile:
	def __init__(self, dbus, session_bus, name="Profile"):
		self.name = name
		self.default_status = "unknown"
		self.runnable = 0

		self.status = -1
		self.status_str = self.default_status
		self.dbus = dbus
		self.session_bus = session_bus
		self.dbus_name = "org.openmoko.qtopia.Phonestatus"
		self.dbus_path = "/Profiles"
		self.dbus_iface = "org.openmoko.qtopia.Profiles"
		#debug("enter init device : " + self.name)

	def dbus_get_value(self, value):
		#debug("value: " + str(value))
		self.status = value[1]
		self.update_label()

	def dbus_set_value(self, value):
		#debug("dbus_set_value: " + str(value))
		pass

	def dbus_error_handler(self, error):
		print "error getting bus names - %s" % str(error)
		self.status = -1
		self.status_str = self.default_status
		self.runnable = 0
		self.update_label()

	def dbus_owner_changed(self, owner):
		#debug("dbus_owner_changed: " + str(owner))

		if not self.runnable and owner <> "":
			self.connect_to_qtopia()

			if self.runnable:
				self.get_status()

	def after_animate_click(self, edje_obj, emission, source):
		#debug('onclick')
		if (self.runnable):
			if (self.status == len(self.profile_array)):
				self.status = 1
			else:
				self.status += 1
			self.set_status()
			self.update_label()

	def init_status(self):
		#debug('init_status')
		self.connect_to_qtopia()

		if self.runnable:
			self.get_status()
		else:
			self.session_bus.watch_name_owner(self.dbus_name, self.dbus_owner_changed)
			self.update_label()

	def connect_to_qtopia(self):
		self.runnable = 1
		try:
			self.qtopia_obj = self.session_bus.get_object(self.dbus_name, self.dbus_path, introspect=False)
		except:
			debug("Can't get Qtopia object:")
			debug(sys.exc_info()[1])
			self.runnable = 0
		else:
			try:
				self.qtopia_iface = self.dbus.Interface(self.qtopia_obj, dbus_interface=self.dbus_iface)
			except:
				debug("Can't get Qtopia interface:")
				debug(sys.exc_info()[1])
				self.runnable = 0
			else:
				self.profile_array = self.qtopia_iface.profiles()

	def get_status(self):
		#debug('get_status')
		self.qtopia_iface.currentProfile(reply_handler=self.dbus_get_value, error_handler=self.dbus_error_handler)

	def set_status(self):
		#debug('set_status: ' + str(self.status))
		self.qtopia_iface.setCurrentProfile(self.profile_array[self.status - 1], reply_handler=self.dbus_set_value, error_handler=self.dbus_error_handler)

	def update_label(self):
		#debug('update_label: ' + str(self.status))
		if self.runnable:
			self.status_str = self.profile_array[self.status - 1][0]
			#debug('update_label: ' + self.status_str)
		self.edje_obj.part_text_set("item-status", self.status_str)
