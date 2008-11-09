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

class time:
	def __init__(self, name="Clock"):
		self.name = name
		self.default_status = ""
		#debug("enter init device : "+self.name)

	def after_animate_click(self, edje_obj, emission, source):
		#debug('onclick')
		#os.popen("qcop service send 'Time' 'editTime()'")
		os.popen("qcop QPE/Application/clock 'raise()'")

	def init_status(self):
		#debug('init_status')
		pass
