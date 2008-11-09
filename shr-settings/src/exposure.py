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
import os


### TODO ###

## modules ##
# GSM -> needs Qtopia dbus call (holger)
# WiFi -> needs network manager (raster)
# Bluetooth -> DONE
# GPS -> DONE
# Backlight -> DONE
# Suspend -> DONE
# Profile -> DONE
# Time -> DONE
# System Info -> DONE
##

## misc ##
# load backlight settings on boot
##

FIFO_PATH = '/tmp/exposure.fifo'
UMASK = 0
WORKDIR = "/"
MAXFD = 1024

myexposure = None


def init_module_status(self):
	for item in self.loaded_modules:
		item.init_status()

	if self.xdesktop.runnable:
		self.xdesktop.init_status()

	## add empty item so that the last item can scroll into the visible screen
	self.last_item_canvas_obj = self.etk.Canvas()
	self.last_item_edje_obj = self.edje.Edje(self.window.evas, file=self.edje_file, group="list_item_unclickable")
	self.last_item_canvas_obj.object_add(self.last_item_edje_obj)
	self.vbox.append(self.last_item_canvas_obj, self.etk.VBox.START, self.etk.VBox.NONE, 0)
	self.last_item_edje_obj.show()

	self.vbox.size_recalc_queue()
	self.vbox.redraw_queue()
	self.vbox.show_all()

class exposure():
	def __init__(self, etk, edje, ecore, name="exposure"):
		self.name = name
		self.etk = etk
		self.edje = edje
		self.ecore = ecore

		self.mainloop = e_dbus.DBusEcoreMainLoop()
		self.session_bus = dbus.SessionBus(mainloop=self.mainloop)
		self.system_bus = dbus.SystemBus(mainloop=self.mainloop)

		### window init ###

		if self.ecore.evas.engine_type_supported_get("software_x11_16"):
			self.window = self.ecore.evas.SoftwareX11_16(w=480, h=575)
		else:
			self.window = self.ecore.evas.SoftwareX11(w=480, h=575)

		self.window.callback_delete_request = self.window_delete_request
		self.window.title = "Settings"

		self.edje_file = "/usr/share/exposure/exposure.edj"
		self.edj = self.edje.Edje(self.window.evas, file=self.edje_file, group="exposure")
		self.edj.size = self.window.evas.size
		self.window.data["edje"] = self.edj
		self.edje.frametime_set(1.0/30)

		self.edj.signal_callback_add("mouse,up,1", "arrow-event", self.arrow_clicked)
		self.edj.show()

		### load and initialize modules ###

		#self.mygsm = gsm()
		self.mybt = bt()
		self.mygps = gps()
		self.mywifi = wifi(self.etk, self.edj, self.edje.Edje, self.edje_file, self.window.evas, dbus, self.system_bus, self.ecore, self.window)
		self.mybrightness = brightness()
		self.mysuspend = suspend(dbus, self.session_bus)
		self.mytime = time()
		self.mysysinfo = sysinfo(self.etk, self.edj, self.edje.Edje, self.edje_file, self.window.evas)
		self.myprofile = profile(dbus, self.session_bus)
		self.xdesktop = xdesktop(self)

		self.loaded_modules = [self.mywifi, self.mybt, self.mygps, self.mybrightness, self.myprofile, self.mysuspend, self.mytime, self.mysysinfo]

		### gui setup ###

		self.vbox = self.etk.VBox()
		self.scrolled_view = self.etk.ScrolledView()
		self.scrolled_view.add_with_viewport(self.vbox)
		## hide scrollbars
		self.scrolled_view.policy_set(2, 2)
		## make it dragable
		self.scrolled_view.dragable_set(1)

		#print "drag_bouncy: " + str(self.scrolled_view.drag_bouncy_get())
		#print "drag_interval: " + str(self.scrolled_view.drag_interval_get())
		#print "drag_damping: " + str(self.scrolled_view.drag_damping_get())

		#self.scrolled_view.drag_interval_set(5.0)
		#self.scrolled_view.drag_damping_set(1000)
		## make it non-bouncing
		self.scrolled_view.drag_bouncy_set(0)

		for item in self.loaded_modules:
			item.timer_add = self.ecore.timer_add
			self.add_vbox_item(item)
			#item.canvas_obj = self.etk.Canvas()
			#item.edje_obj = self.edje.Edje(self.window.evas, file=self.edje_file, group="list_item")
			#item.canvas_obj.object_add(item.edje_obj)
			#item.edje_obj.signal_callback_add("after_animate_click", "list_item", item.after_animate_click)
			#item.edje_obj.part_text_set("item-label", item.name)
			#self.vbox.append(item.canvas_obj, self.etk.VBox.START, self.etk.VBox.NONE, 0)
			#item.edje_obj.show()

		self.mainmenu_win = self.etk.Embed(self.window.evas)
		self.mainmenu_win.add(self.scrolled_view)
		self.mainmenu_win.show_all()
		self.edj.part_swallow("main-menu", self.mainmenu_win.object)

		self.sysinfo_win = self.etk.Embed(self.window.evas)
		self.sysinfo_win.add(self.mysysinfo.scrolled_view)
		self.sysinfo_win.show_all()
		self.edj.part_swallow("sysinfo", self.sysinfo_win.object)

		self.wifi_win = self.etk.Embed(self.window.evas)
		self.wifi_win.add(self.mywifi.scrolled_view)
		self.wifi_win.show_all()
		self.edj.part_swallow("wifi", self.wifi_win.object)

	def run(self):
		### minimal init ###
		#for item in self.loaded_modules:
			#item.edje_obj.part_text_set("item-status", item.default_status)

		self.window.show()
		init_module_status(self)

		#self.ecore.timer_add(0.5, init_module_status, self)

		#self.ecore.main_loop_begin()
		#self.window.hide()
		#self.ecore.main_loop_iterate()

	def focus(self):
		self.window.activate()

	def stop(self):
		self.ecore.main_loop_quit()
		self.restart_deamon()

	def window_delete_request(self, evas_obj):
		self.ecore.main_loop_quit()
		# avoid loop
		self.window.callback_delete_request = None
		self.restart_deamon()

	def restart_deamon(self):
		self.window.hide()
		self.ecore.main_loop_iterate()
		os.unlink(FIFO_PATH)
		os.system(sys.argv[0])

	def add_vbox_item(self, item):
		item.canvas_obj = self.etk.Canvas()
		item.edje_obj = self.edje.Edje(self.window.evas, file=self.edje_file, group="list_item")
		item.canvas_obj.object_add(item.edje_obj)
		item.edje_obj.signal_callback_add("after_animate_click", "list_item", item.after_animate_click)
		item.edje_obj.part_text_set("item-label", item.name)
		item.edje_obj.part_text_set("item-status", item.default_status)
		self.vbox.append(item.canvas_obj, self.etk.VBox.START, self.etk.VBox.NONE, 0)
		item.edje_obj.show()

	def arrow_clicked(self, edje_obj, emission, source):
		self.edj.part_text_set("settings-text", "Settings")


def createDaemon():
	try:
		pid = os.fork()
	except OSError, e:
		raise Exception, "%s [%d]" % (e.strerror, e.errno)

	## first child
	if (pid == 0):
		## make it session leader
		os.setsid()

		try:
			pid = os.fork()
		except OSError, e:
			raise Exception, "%s [%d]" % (e.strerror, e.errno)

		## second child
		if (pid == 0):
			os.chdir(WORKDIR)
			os.umask(UMASK)
		else:
			## second parent
			os._exit(0)

	else:
		## first parent
		os._exit(0)

	## daemon process
	# Iterate through and close all file descriptors.
	for fd in range(0, MAXFD):
		try:
			os.close(fd)
		## ignore error (unopened fds)
		except OSError:
			pass

	## redirect stdin, stdout and stderr to /dev/null
	os.open("/dev/null", os.O_RDWR)
	os.dup2(0, 1)
	os.dup2(0, 2)

	return(0)

def fifo_reader_exists(cmd):
	result = 0
	child = os.popen("fuser " + FIFO_PATH)
	#print "child: >" + child.read() + "<"
	if child.read() <> "":
		result = 1
	child.close()
	return result

def cb_fifo_read(fd_handler):
	data = os.read(fd_handler.fd, 50)

	global myexposure

	#print "got: " + data
	if data == "run":
		if myexposure == None:
			myexposure = exposure(etk, edje, ecore)
			myexposure.run()
		else:
			myexposure.focus()
	elif data == "stop":
		if myexposure != None:
			myexposure.stop()
			return False
	else:
		print "unknown command: " + data

	return True

def write_to_fifo(cmd):
	fifo_fh = open(FIFO_PATH, 'w+')
	fifo_fh.write(cmd)
	fifo_fh.close()

if __name__ == "__main__":

	cmd = "run"
	do_fork = 1
	myargv = sys.argv[1:]

	if (len(myargv) > 0) and (myargv[0] == "-f"):
		myargv.pop(0)
		do_fork = 0

	if len(myargv) > 0:
		cmd = myargv[0]

	if os.path.exists(FIFO_PATH) and fifo_reader_exists(cmd):
		write_to_fifo(cmd)
		sys.exit(0)

	elif os.path.exists(FIFO_PATH):
		os.unlink(FIFO_PATH)

	if do_fork:
		createDaemon()

	os.mkfifo(FIFO_PATH)
	fifo_fh = open(FIFO_PATH, 'r+')

	import etk
	import edje
	import ecore.evas

	import dbus
	import e_dbus
	from expos import gsm, bt, gps, brightness, suspend, time, profile, sysinfo, wifi, xdesktop

	fifo_read = ecore.fd_handler_add(fifo_fh, ecore.ECORE_FD_READ, cb_fifo_read)

	if len(myargv) > 0:
		ecore.timer_add(0.5, write_to_fifo, cmd)

	#import profile # conflicts with our profile module
	#profile.run('ecore.main_loop_begin()', 'out_file.prof')

	ecore.main_loop_begin()

