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
import sys
import ConfigParser
import ecore.x


def debug(msg):
	print msg

def connect_to_wifi_timeout(self):
	#debug('connect_to_wifi_timeout')
	self.connection_status = "error"
	self.edj_obj.signal_emit("error-window-out", "exposure")


class wifi_network:
	def __init__(self,wifi_obj, edje_group):
		self.wifi_obj = wifi_obj
		self.edje_group = edje_group

	def init_canvas(self, uid, essid, snr, secure):
		self.canvas_obj = self.wifi_obj.etk_obj.Canvas()
		self.edje_obj = self.wifi_obj.edje(self.wifi_obj.canvas, file=self.wifi_obj.edje_file, group=self.edje_group)
		self.canvas_obj.object_add(self.edje_obj)
		self.edje_obj.calc_force()
		#print "size_get (" + self.uid + "): " + str(self.edje_obj.size_get())
		#print "size_max_get: " + str(self.edje_obj.size_max_get())
		#print "size_min_calc: " + str(self.edje_obj.size_min_calc())
		#print "size_min_get: " + str(self.edje_obj.size_min_get())
		#print dir(self.edje_obj)
		#self.edje_obj.size = (440,80)
		#self.canvas_obj.size_request_set(self.edje_obj.size[0], self.edje_obj.size[1])
		self.edje_obj.signal_callback_add("after_animate_click", "list_item", self.onclick)
		self.edje_obj.show()

		if (uid == "list_item"):
			self.edje_obj.part_text_set("item-status", self.wifi_obj.status_str)

		self.update_wifi_network(uid, essid, snr, secure)

	def update_wifi_network(self, uid, essid, snr, secure):
		self.uid = uid
		self.essid = essid
		self.snr = snr
		self.secure = secure

		if (uid == "list_item"):
			return

		self.edje_obj.part_text_set("network-name", self.essid)
		self.edje_obj.part_text_set("snr-label", str(self.snr) + "%")
		## 1 / float -> set snr
		self.edje_obj.message_send(1, float(self.snr)/100.0)
		## 1 / int -> is secure
		self.edje_obj.message_send(1, self.secure)

	def destroy_canvas(self):
		self.edje_obj.delete()
		self.canvas_obj.destroy()

	def onclick(self, edje_obj, emission, source):
		#debug('wifi_clicked: ' + self.uid)

		# abort if the error message is visible
		if (self.wifi_obj.connection_status == "error"):
			return

		if (self.uid == "list_item"):
			self.list_item_onclick()
		else:
			# abort if we trying to connect
			if (self.wifi_obj.connection_status == "connecting"):
				return

			# we already are connected to this network
			if (self.wifi_obj.connection_status == "connected") and (self.wifi_obj.curr_network_obj == self):
				return

			#debug('changing network: ' + self.essid)
			self.wifi_obj.last_network_obj = self.wifi_obj.curr_network_obj
			self.wifi_obj.curr_network_obj = self

			if self.secure:
				if (self.wifi_obj.wifi_config.has_section(self.uid)) and (self.wifi_obj.wifi_config.get(self.uid, "password") <> ""):
					self.wifi_obj.pw_field.text_set(self.wifi_obj.wifi_config.get(self.uid, "password"))
					self.wifi_obj.init_wifi_connection()
				else:
					self.wifi_obj.edj_obj.part_text_set("settings-text", "Settings / WiFi / Enter Password")
					self.wifi_obj.edj_obj.signal_emit("enter_pw-slide-in", "exposure")
					self.wifi_obj.pw_field.text_set("")
					self.wifi_obj.pw_field.focus()
			else:
				self.wifi_obj.init_wifi_connection()

	def list_item_onclick(self):
		#debug('list_item_onclick')
		if self.wifi_obj.runnable:
			if (self.wifi_obj.status == 1):
				self.wifi_obj.status = 0
				self.wifi_obj.status_str = "off"
				self.wifi_obj.del_all_networks()
				self.wifi_obj.wlan_iface.SetPolicy("off", reply_handler=self.wifi_obj.dbus_set_if_status, error_handler=self.wifi_obj.dbus_error_handler)
			else:
				self.wifi_obj.status = 1
				self.wifi_obj.status_str = "on"
				self.wifi_obj.wlan_iface.SetPolicy("auto", reply_handler=self.wifi_obj.dbus_set_if_status, error_handler=self.wifi_obj.dbus_error_handler)
			#debug('list_item_onclick: ' + self.wifi_obj.status_str)
			self.edje_obj.part_text_set("item-status", self.wifi_obj.status_str)


class wifi:
	def __init__(self, etk_obj, edj_obj, edje, edje_file, canvas, dbus, system_bus, ecore, window, name="WiFi"):
		self.name = name
		self.default_status = ""
		self.runnable = 0

		self.status = -1
		self.status_str = "unknown"
		self.settings_text = "Settings / WiFi"
		self.dbus = dbus
		self.system_bus = system_bus
		self.dbus_name = "org.freedesktop.connman"
		self.dbus_path = "/"
		self.dbus_manager_iface = "org.freedesktop.connman.Manager"
		self.dbus_wlan_iface = "org.freedesktop.connman.Interface"

		self.wlan_obj = None
		self.curr_network_obj = None
		self.last_network_obj = None
		self.connection_status = "scanning"
		self.network_list = {}
		self.current_network = {} #self.dbus.Dictionary(signature='a{s(s)}')
		self.num_networks = 0
		self.wifi_config_path = "/etc/wifi.conf"
		self.wifi_config = ConfigParser.SafeConfigParser()
		self.wifi_config.read(self.wifi_config_path)

		## gui ##
		self.etk_obj = etk_obj
		self.edj_obj = edj_obj
		self.edje_file = edje_file
		self.edje = edje
		self.canvas = canvas
		self.ecore = ecore
		self.window = window

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
		## make it non-bouncing
		self.scrolled_view.drag_bouncy_set(0)

		## add empty item so that the last item can scroll into the visible screen
		self.last_item_canvas_obj = self.etk_obj.Canvas()
		self.last_item_edje_obj = self.edje(self.canvas, file=self.edje_file, group="list_item_unclickable")
		self.last_item_canvas_obj.object_add(self.last_item_edje_obj)
		self.last_item_edje_obj.show()
		self.vbox.append(self.last_item_canvas_obj, self.etk_obj.VBox.START, self.etk_obj.VBox.NONE, 0)
		#debug("enter init device : " + self.name)

	def dbus_get_if_list(self, list_array):
		#debug("dbus_get_if_list: " + str(list_array))
		for net_if in list_array:
			#debug("dbus_get_if_list: " + str(net_if))
			obj = self.system_bus.get_object(self.dbus_name, net_if, introspect=False)
			props_hash = obj.GetProperties(dbus_interface=self.dbus_wlan_iface)

			## wifi interface (we support only one interface)
			if (props_hash['Type'] == "80211") and (self.wlan_obj == None):
				self.wlan_obj = self.system_bus.get_object(self.dbus_name, net_if, introspect=True)
				self.wlan_iface = self.dbus.Interface(self.wlan_obj, dbus_interface=self.dbus_wlan_iface)
				self.wlan_iface.connect_to_signal("NetworkFound", self.dbus_network_found_cb)
				self.wlan_iface.connect_to_signal("NetworkChanged", self.dbus_network_found_cb)
				self.wlan_iface.connect_to_signal("IPv4Changed", self.dbus_network_connected)
				self.wlan_iface.GetPolicy(reply_handler=self.dbus_get_if_status, error_handler=self.dbus_error_handler)
				break

	def dbus_get_if_status(self, value):
		#debug("dbus_get_if_status: " + str(value))
		if (value == "auto"):
			self.status = 1
			self.status_str = "on"
		else:
			self.status = 0
			self.status_str = "off"

		self.network_list["list_item"].edje_obj.part_text_set("item-status", self.status_str)

	def dbus_set_if_status(self):
		#debug("dbus_set_if_status: " + self.wlan_iface.GetPolicy())
		pass

	def dbus_error_handler(self, error):
		debug("dbus error - %s" % str(error))
		self.status = -1
		self.status_str = "unknown"
		self.runnable = 0
		self.del_all_networks()
		self.network_list["list_item"].edje_obj.part_text_set("item-status", self.status_str)

	def dbus_network_found_cb(self, network_hash):
		#debug("dbus_network_found_cb: " + str(network_hash))
		if (self.status == 1):
			if not ("ESSID" in network_hash):
				return

			if not ("BSSID" in network_hash):
				return

			if ("Security" in network_hash):
				secure_net = 1
			else:
				secure_net = 0

			self.add_network("wifi_item", network_hash["BSSID"], network_hash["ESSID"].replace("\t", " "), network_hash["Signal"], secure_net, self.num_networks)
			#debug("network: " + network_hash["BSSID"] + " <> |" + network_hash["ESSID"] + "|")

	def dbus_set_network(self):
		#debug("dbus_set_network: " + str(self))
		pass

	def dbus_network_connected(self):
		#debug("dbus_network_connected: " + str(self))
		# we got connected in time
		self.connection_timer.delete()
		## 2 / int -> is selected
		self.curr_network_obj.edje_obj.message_send(2, 1)
		self.connection_status = "connected"

		if self.curr_network_obj.secure:
			if not self.wifi_config.has_section(self.curr_network_obj.uid):
				self.wifi_config.add_section(self.curr_network_obj.uid)

			self.wifi_config.set(self.curr_network_obj.uid, "description", self.current_network["ESSID"])
			self.wifi_config.set(self.curr_network_obj.uid, "password", self.current_network["PSK"])

			config_fd = open(self.wifi_config_path, "w")
			self.wifi_config.write(config_fd)
			config_fd.close()

	def init_status(self):
		#debug('init_status')
		self.del_all_networks()
		self.add_network("list_item", "list_item", "", "", "", 0)

		self.runnable = 1
		try:
			self.connman_obj = self.system_bus.get_object(self.dbus_name, self.dbus_path, introspect=False)
		except:
			print "Can't get connman object:"
			print sys.exc_info()[1]
			self.runnable = 0
		else:
			try:
				self.nm_iface = self.dbus.Interface(self.connman_obj, dbus_interface=self.dbus_manager_iface)
			except:
				print "Can't get connman interface:"
				print sys.exc_info()[1]
				self.runnable = 0

		if (self.runnable):
			# generate pw window
			self.pw_canvas_obj = self.etk_obj.Canvas()
			self.pw_edj_obj = self.edje(self.canvas, file=self.edje_file, group="enter_pw-window")
			self.pw_canvas_obj.object_add(self.pw_edj_obj)
			self.pw_edj_obj.show()
			self.pw_win = self.etk_obj.Embed(self.window.evas)
			self.pw_win.add(self.pw_canvas_obj)
			self.pw_win.show_all()
			self.edj_obj.part_swallow("enter_pw", self.pw_win.object)

			self.pw_field_embed = self.etk_obj.Embed(self.window.evas)
			self.pw_field = self.etk_obj.Entry()
			self.pw_field.password_mode = True
			self.pw_field_embed.add(self.pw_field)
			self.pw_field_embed.show_all()
			self.pw_edj_obj.part_swallow("enter_pw_widget", self.pw_field_embed.object)

			self.err_window_canvas_obj = self.etk_obj.Canvas()
			self.err_window_edj_obj = self.edje(self.canvas, file=self.edje_file, group="error_window")
			self.err_window_canvas_obj.object_add(self.err_window_edj_obj)
			self.err_window_edj_obj.show()
			self.err_win = self.etk_obj.Embed(self.window.evas)
			self.err_win.add(self.err_window_canvas_obj)
			self.err_win.show_all()
			self.edj_obj.part_swallow("error_window", self.err_win.object)
			self.err_window_edj_obj.signal_callback_add("error-window-default", "exposure", self.error_ok_callback)

			self.pw_edj_obj.signal_callback_add("mouse,up,1", "ok-button-mouse-catcher", self.pw_win_close)
			self.pw_edj_obj.signal_callback_add("mouse,up,1", "cancel-button-mouse-catcher", self.pw_win_close)
			self.edj_obj.signal_callback_add("kbd_show_signal", "kbd", self.kbd_show)
			self.edj_obj.signal_callback_add("kbd_hide_signal", "kbd", self.kbd_hide)

			self.nm_iface.ListInterfaces(reply_handler=self.dbus_get_if_list, error_handler=self.dbus_error_handler)

	def after_animate_click(self, edje_obj, emission, source):
		#debug('onclick')
		self.edj_obj.signal_emit("wifi-slide-in", "exposure")
		self.edj_obj.part_text_set("settings-text", self.settings_text)

		# we left the wifi screen before clicking the error message away which is now invisible
		if (self.connection_status == "error"):
			self.connection_status = "scanning"

	def error_ok_callback(self, edje_obj, emission, source):
		self.connection_status = "scanning"
		self.edj_obj.signal_emit("error-window-default", "exposure")

	def add_network(self, edje_group, uid, essid, snr, secure, index):
		#debug('add_network: ' + uid + "; index/num_networks: " + str(index) + "/" + str(self.num_networks))
		if not (uid in self.network_list) or not self.network_list[uid].visible:
			if not (uid in self.network_list):
				self.network_list[uid] = wifi_network(self, edje_group)

			self.network_list[uid].init_canvas(uid, essid, snr, secure)
			self.vbox.insert_at(self.network_list[uid].canvas_obj, self.etk_obj.VBox.START, index, self.etk_obj.VBox.NONE, 0)
			self.network_list[uid].visible = 1
			self.num_networks += 1
		else:
			self.network_list[uid].update_wifi_network(uid, essid, snr, secure)

		self.redraw_network_list()

	def del_all_networks(self):
		#debug('del_all_networks')
		for uid, network in self.network_list.iteritems():
			self.del_network(uid, network)
		self.redraw_network_list()

	def del_network(self, uid, network):
		#debug('del_network: ' + str(uid))
		if (uid == "list_item"):
			return

		if not (uid in self.network_list):
			return

		if (self.network_list[uid].visible == 0):
			return

		network.destroy_canvas()
		network.visible = 0
		self.num_networks -= 1

	def redraw_network_list(self):
		#debug('redraw_network_list')
		self.vbox.size_recalc_queue()
		self.vbox.redraw_queue()
		self.vbox.show_all()

	def kbd_show(self, edje_obj, emission, source):
		self.window.x_window_virtual_keyboard_state_set(ecore.x.ECORE_X_VIRTUAL_KEYBOARD_STATE_ON)

	def kbd_hide(self, edje_obj, emission, source):
		self.window.x_window_virtual_keyboard_state_set(ecore.x.ECORE_X_VIRTUAL_KEYBOARD_STATE_OFF)

	def pw_win_close(self, edje_obj, emission, source):
		self.edj_obj.part_text_set("settings-text", self.settings_text)
		self.edj_obj.signal_emit("enter_pw-slide-out", "exposure")

		if (source == "ok-button-mouse-catcher"):
			self.init_wifi_connection()

	def init_wifi_connection(self):
		if (self.curr_network_obj.secure):
			self.current_network["PSK"] = self.pw_field.text_get()
			#print "pw: " + self.current_network["PSK"]
			#self.current_network["PSK"] = "0p3nm0k0"
			#print "pw: " + self.current_network["PSK"]
		else:
			self.current_network["PSK"] = ""

		#self.wifi_obj.current_network["BSSID"] = self.wifi_obj.curr_network_obj.uid
		self.current_network["ESSID"] = self.curr_network_obj.essid

		# unmark currently selected network
		if (self.last_network_obj <> None):
			## 2 / int -> is selected
			self.last_network_obj.edje_obj.message_send(2, 0)

		self.connection_status = "connecting"
		self.connection_timer = self.ecore.timer_add(60, connect_to_wifi_timeout, self)

		self.wlan_iface.SetNetwork(self.current_network, reply_handler=self.dbus_set_network, error_handler=self.dbus_error_handler)

