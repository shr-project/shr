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

import os


def debug(msg):
	print msg

class brightness:
	def __init__(self, name="Brightness"):
		self.name = name
		self.default_status = "unknown"
		self.runnable = 0

		self.brightness = 0
		self.status_str = self.default_status
		self.brightness_path = "/sys/class/backlight/pcf50633-bl/brightness"
		## safe the config ?!
		#self.brightness_cpath = "/etc/brightness.conf"
		#debug("enter init device : " + self.name)

	def set_brightness(self, value):
		#debug('set_brightness')
		self.brightness = int(value)
		os.system("echo " + value + " > " + self.brightness_path)
		#os.system("echo " + value + " > " + self.brightness_cpath)

	def after_animate_click(self, edje_obj, emission, source):
		#debug('onclick')
		if self.runnable:
			if (self.brightness < 22):
				self.set_brightness('42')
				self.status_str = "medium"
			elif (self.brightness > 43):
				self.set_brightness('21')
				self.status_str = "low"
			else:
				self.set_brightness('63')
				self.status_str = "high"
			self.update_label()

	def init_status(self):
		#debug('init_status')
		self.runnable = 1
		if not (os.path.isfile(self.brightness_path)):
			self.runnable = 0
			self.status_str = "unknown"
		else:
			self.get_status()
		self.update_label()

	def get_status(self):
		#debug('get_status')
		file = open(self.brightness_path)
		self.brightness = int(file.readline())
		if (self.brightness < 22):
			self.status_str = "low"
		elif (self.brightness > 43):
			self.status_str = "high"
		else:
			self.status_str = "medium"
		file.close()

	def update_label(self):
		#debug('update_label')
		self.edje_obj.part_text_set("item-status", self.status_str)
