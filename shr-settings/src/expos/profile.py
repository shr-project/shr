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
	def __init__(self, dbus, system_bus, name="Profile"):
		self.name = name
		self.default_status = "unknown"
		self.runnable = 0

		self.status = -1
		self.status_str = self.default_status
		self.dbus = dbus
		self.system_bus = system_bus
		self.dbus_name = "org.freesmartphone.opreferencesd"
		self.dbus_path = "/org/freesmartphone/Preferences"
		self.dbus_iface = "org.freesmartphone.Preferences"
		debug("enter init device : " + self.name)

	def dbus_get_profiles(self, profiles):
		self.profile_array = profiles
		debug("PROFILES: %s" % profiles)

	def dbus_get_value(self, value):
		debug("value: " + str(value))
		self.status = self.profile_array.index(value)
		self.update_label()

	def dbus_set_value(self):
		debug("dbus_set_value")
		pass

	def dbus_error_handler(self, error):
		print "error getting bus names - %s" % str(error)
		self.status = -1
		self.status_str = self.default_status
		self.runnable = 0
		self.update_label()

	def dbus_owner_changed(self, owner):
		debug("dbus_owner_changed: " + str(owner))

		if not self.runnable and owner <> "":
			self.connect_to_frameworkd()

			if self.runnable:
				self.get_status()

	def after_animate_click(self, edje_obj, emission, source):
		debug('onclick')
		if (self.runnable):
			if (self.status == len(self.profile_array) - 1):
				self.status = 0
			else:
				self.status += 1
			self.set_status()
			self.update_label()

	def init_status(self):
		#debug('init_status')
		self.connect_to_frameworkd()

		if self.runnable:
			self.get_status()
		else:
			self.system_bus.watch_name_owner(self.dbus_name, self.dbus_owner_changed)
			self.update_label()

	def connect_to_frameworkd(self):
		self.runnable = 1
		try:
			self.frameworkd_obj = self.system_bus.get_object(self.dbus_name, self.dbus_path, introspect=False)
		except:
			debug("Can't get frameworkd object:")
			debug(sys.exc_info()[1])
			self.runnable = 0
		else:
			try:
				self.frameworkd_iface = self.dbus.Interface(self.frameworkd_obj, dbus_interface=self.dbus_iface)
			except:
				debug("Can't get frameworkd interface:")
				debug(sys.exc_info()[1])
				self.runnable = 0
			else:
				self.frameworkd_iface.GetProfiles(reply_handler=self.dbus_get_profiles, error_handler=self.dbus_error_handler)

	def get_status(self):
		debug('get_status')
		self.frameworkd_iface.GetProfile(reply_handler=self.dbus_get_value, error_handler=self.dbus_error_handler)

	def set_status(self):
		debug('set_status: ' + str(self.status))
		self.frameworkd_iface.SetProfile(self.profile_array[self.status], reply_handler=self.dbus_set_value, error_handler=self.dbus_error_handler)

	def update_label(self):
		debug('update_label: ' + str(self.status))
		if self.runnable:
			self.status_str = self.profile_array[self.status]
			debug('update_label: ' + self.status_str)
		self.edje_obj.part_text_set("item-status", self.status_str)

