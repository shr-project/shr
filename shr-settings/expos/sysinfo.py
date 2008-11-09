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

class sysinfo_item:
	def __init__(self, etk_obj, edje, canvas, edje_file, edje_group):
		self.canvas_obj = etk_obj.Canvas()
		self.edje_obj = edje(canvas, file=edje_file, group=edje_group)
		self.canvas_obj.object_add(self.edje_obj)
		self.edje_obj.calc_force()
		self.edje_obj.show()

def _uboot_version(self):
	child = os.popen(self.uboot_cmd)
	self.item_list[self.uboot_index].edje_obj.part_text_set("item-label", child.read().replace("\n", ""))
	child.close()

class sysinfo:
	def __init__(self, etk_obj, edj_obj, edje, edje_file, canvas, name="System Info"):
		self.name = name
		self.default_status = ""
		self.runnable = 0

		## gui ##
		self.etk_obj = etk_obj
		self.edj_obj = edj_obj
		self.edje_file = edje_file
		self.edje = edje
		self.canvas = canvas

		self.vbox = self.etk_obj.VBox()
		self.alignment = self.etk_obj.Alignment()
		self.alignment.set(1.0, 1.0, 1.0, 1.0)
		self.alignment.add(self.vbox)
		self.scrolled_view = self.etk_obj.ScrolledView()
		self.scrolled_view.add_with_viewport(self.alignment)
		## hide scrollbars
		self.scrolled_view.policy_set(2, 2)
		## make it dragable
		self.scrolled_view.dragable_set(1)

		self.data_file = "/etc/om-version"
		self.uboot_file = "/dev/mtdblock0"
		self.uboot_cmd = "strings " + self.uboot_file +  " | grep 'Bootloader U-Boot'"
		self.uboot_index = 11
		self.item_list = []
		#debug("enter init device : " + self.name)

	def init_status(self):
		#debug('init_status')
		for item in ["Version", "Build ID", "Branch", "Build Host", "Time Stamp", "U-Boot"]:
			list_item = sysinfo_item(self.etk_obj, self.edje, self.canvas, self.edje_file, "sysinfo_item_big")
			self.vbox.append(list_item.canvas_obj, self.etk_obj.VBox.START, self.etk_obj.VBox.NONE, 0)
			list_item.edje_obj.part_text_set("item-label", item)
			self.item_list.append(list_item)

			list_item = sysinfo_item(self.etk_obj, self.edje, self.canvas, self.edje_file, "sysinfo_item_small")
			self.vbox.append(list_item.canvas_obj, self.etk_obj.VBox.START, self.etk_obj.VBox.NONE, 0)
			list_item.edje_obj.part_text_set("item-label", "")
			self.item_list.append(list_item)

		self.vbox.size_recalc_queue()
		self.vbox.redraw_queue()
		self.vbox.show_all()

		self.runnable = 1
		if not (os.path.isfile(self.data_file)):
			self.runnable = 0
		if not (os.path.exists(self.uboot_file)):
			print "is not uboot: " + self.uboot_file
			self.runnable = 0

	def after_animate_click(self, edje_obj, emission, source):
		#debug('onclick')
		self.get_om_version()
		self.edj_obj.signal_emit("sysinfo-slide-in", "exposure")
		self.edj_obj.part_text_set("settings-text", "Settings / System Information")

		if self.runnable:
			## load uboot version string only once - it is unlikely to change
			if self.item_list[self.uboot_index].edje_obj.part_text_get("item-label") == "":
				self.timer_add(1.5, _uboot_version, self)

	def get_om_version(self):
		#debug('get_om_version')
		if self.runnable:
			file = open(self.data_file)
			version_info = file.readlines()
			for index, line in enumerate(version_info):
				self.item_list[(index * 2 + 1)].edje_obj.part_text_set("item-label", line.partition(':')[2].strip())
				if (index == 4):
					break
			file.close()
