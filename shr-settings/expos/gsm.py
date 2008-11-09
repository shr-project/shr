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

### delayed init sequence for the gsm modem ###
def _power_on_first(self):
	os.system("echo 0 > " + self.pows)
	self.timer_add(1, _power_on_sec, self)

def _power_on_sec(self):
	os.system("echo 1 > " + self.pows)
	self.timer_add(1, _power_on_third, self)

def _power_on_third(self):
	os.system("echo 1 > " + self.res)
	self.timer_add(1, _power_on_fourth, self)

def _power_on_fourth(self):
	os.system("echo 0 > " + self.res)
	self.timer_add(2, _power_on_fifth, self)

def _power_on_fifth(self):
	self.timer_running = 0
	if (self.timer_mode == 0):
		self.power_off

class gsm:
	def __init__(self, name="GSM"):
		self.name = name
		self.default_status = "Qt Bug"
		self.runnable = 0

		self.status = 0
		self.status_str = self.default_status
		self.timer_running = 0
		self.timer_mode = 0
		self.pows = "/sys/bus/platform/devices/neo1973-pm-gsm.0/power_on"
		self.res = "/sys/bus/platform/devices/neo1973-pm-gsm.0/reset"
		self.dl = "/sys/bus/platform/devices/neo1973-pm-gsm.0/download"
		#debug("enter init device : " + self.name)

	def power_on(self):
		#debug('gsm on')
		self.timer_mode = 1
		if (self.timer_running == 0):
			self.timer_running = 1
			self.timer_add(0.1, _power_on_first, self)

	def power_off(self):
		#debug('gsm off')
		self.timer_mode = 0
		if (self.timer_running == 0):
			os.system("echo 0 > " + self.pows)

	def after_animate_click(self, edje_obj, emission, source):
		#debug('onclick')
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
