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

import os


def debug(msg):
	print msg

class gps:
	def __init__(self, name="GPS"):
		self.name = name
		self.default_status = "unknown"
		self.runnable = 0

		self.status = 0
		self.status_str = self.default_status
		self.pows =	\
		'/sys/bus/platform/drivers/neo1973-pm-gps/neo1973-pm-gps.0/pwron'
		#debug("enter init device : "+self.name)

	def power_on(self):
		#debug('gps on')
		os.system("echo 1 > " + self.pows)

	def power_off(self):
		#debug('gps off')
		os.system("echo 0 > " + self.pows)

	def after_animate_click(self, edje_obj, emission, source):
		#debug('onclick')
		if self.runnable:
			if (self.status):
				self.power_off()
				self.status = 0
				self.status_str = "off"
			else:
				self.power_on()
				self.status = 1
				self.status_str = "on"
			self.update_label()

	def init_status(self):
		#debug('init_status')
		self.runnable = 1
		if not (os.path.isfile(self.pows)):
			self.runnable = 0
		if self.runnable:
			self.get_status()
		else:
			self.status_str = self.default_status
		self.update_label()

	def get_status(self):
		#debug('get_status')
		file = open(self.pows)
		status = file.readline()
		if status.find('0') > -1:
			self.status = 0
			self.status_str = "off"
		else:
			self.status = 1
			self.status_str = "on"
		file.close()

	def update_label(self):
		#debug('update_label')
		self.edje_obj.part_text_set("item-status", self.status_str)
