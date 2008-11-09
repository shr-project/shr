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
from xdg.DesktopEntry import *


def debug(msg):
	print msg

class xdesktop:
	def __init__(self, exposure, name="xdesktop-parser"):
		self.name = name
		self.runnable = 1

		self.adv_option_file = "/etc/exposure_adv_opt"
		self.xdesktop_root_folder = "/usr/share/applications"
		self.category1 = "System"
		self.category2 = "Settings"

		self.found_files = []
		self.exposure = exposure

		if not (os.path.isfile(self.adv_option_file)):
			self.runnable = 0
		#debug("enter init device : " + self.name)

	def checkfiles(self, path):
		if os.path.isdir(path):
			ls = os.listdir(path)
			for file in ls:
				self.checkfiles(os.path.join(path, file))
		else:
			entry = DesktopEntry()
			try:
				entry.parse(path)
			except ParsingError, e:
				print "Parsing error: " + str(e)
				return

			category = entry.getCategories()

			if (len(category) < 2) or (category[0] <> self.category1) or (category[1] <> self.category2):
				return

			entry.name = entry.getName()
			entry.default_status = ""
			entry.after_animate_click = self.after_animate_click

			#debug("xdesktop file found: " + entry.name)
			#debug("category: " + str(entry.getCategories()))

			self.found_files.append(entry)

	def after_animate_click(self, edje_obj, emission, source):
		#debug('onclick')
		entry_name = edje_obj.part_text_get("item-label")

		for xdesktop_obj in self.found_files:
			if (xdesktop_obj.name == entry_name):
				os.popen(xdesktop_obj.getExec())

	def init_status(self):
		#debug('init_status')
		self.checkfiles(self.xdesktop_root_folder)

		for xdesktop_obj in self.found_files:
			self.exposure.add_vbox_item(xdesktop_obj)

