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

class suspend:
	def __init__(self, dbus, system_bus, name="Suspend"):
		self.name = name
		self.default_status = "unknown"
		self.runnable = 0

		self.status = -1
		self.status_str = self.default_status
		self.dbus = dbus
		self.system_bus = system_bus
		self.dbus_name = "org.freesmartphone.odeviced"
		self.dbus_path = "/org/freesmartphone/Device/IdleNotifier/0"
		self.dbus_iface = "org.freesmartphone.Device.IdleNotifier"
		#debug("enter init device : " + self.name)

	def dbus_get_value(self, states):
		#debug("dbus_get_value: " + str(value)
		self.status = states["suspend"]
		self.update_label()

	def dbus_set_value(self):
		#debug("dbus_set_value: " + str(self))
		pass

	def dbus_error_handler(self, error):
		print "error getting bus names - %s" % str(error)
		self.status = -1
		self.status_str = "unknown"
		self.runnable = 0
		self.update_label()

	def after_animate_click(self, edje_obj, emission, source):
		#debug('onclick')
		if (self.runnable):
			if (self.status == 0):
				self.set_status(10)
			elif (self.status == 10):
				self.set_status(30)
			elif (self.status == 30):
				self.set_status(60)
			elif (self.status == 60):
				self.set_status(0)
			self.update_label()

	def init_status(self):
		#debug('init_status')
		self.runnable = 1
		try:
			self.e_obj = self.system_bus.get_object(self.dbus_name, self.dbus_path, introspect=False)
		except:
			print "Can't get frameworkd object:"
			print sys.exc_info()[1]
			self.runnable = 0
		else:
			try:
				self.frameworkd_iface = self.dbus.Interface(self.e_obj, dbus_interface=self.dbus_iface)
			except:
				print "Can't get frameworkd interface:"
				print sys.exc_info()[1]
				self.runnable = 0
		if self.runnable:
			self.get_status()
		else:
			self.update_label()

	def get_status(self):
		#debug('get_status')
		self.frameworkd_iface.GetTimeouts(reply_handler=self.dbus_get_value, error_handler=self.dbus_error_handler)

	def set_status(self, value):
		#debug('set_status: ' + str(value))
		self.frameworkd_iface.SetTimeout("suspend", int(value), reply_handler=self.dbus_set_value, error_handler=self.dbus_error_handler)
		self.status = value

	def update_label(self):
		#debug('update_label: ' + str(self.status))
		if self.runnable:
			if (self.status == 0):
				self.status_str = "off"
			elif (self.status == 10) or (self.status == 30) or (self.status == 60):
				self.status_str = str(self.status) + " sec"
			else:
				self.set_status(60)
				self.status_str = str(self.status) + " sec"
		self.edje_obj.part_text_set("item-status", self.status_str)
