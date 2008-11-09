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
import socket
import types
from threading import Thread
#import profile
import gc
import inspect


UMASK = 0
WORKDIR = "/"
MAXFD = 1024
UNIX_PATH = "/tmp/app-launcherd.sock"


# The standard I/O file descriptors are redirected to /dev/null by default.
if (hasattr(os, "devnull")):
	REDIRECT_TO = os.devnull
else:
	REDIRECT_TO = "/dev/null"

def print_mem_leaks():
	gc.collect()

	#print "garbage objects:"
	#for x in gc.garbage:
		#s = str(x)
		#if len(s) > 80:
			#s = s[:80]
		#print type(x),"\n  ", s

	print "object count:"
	obj_hash_count = {}
	obj_hash_filename = {}

	oo = gc.get_objects()
	for o in oo:
		if getattr(o, "__class__", None):
			name = o.__class__.__name__
			if not (name in obj_hash_count):
				try:
					filename = inspect.getabsfile(o.__class__)
					obj_hash_count[name] = 1
					obj_hash_filename[name] = filename
					#print "Object of class:", name, "...",
					#print "defined in file:", filename
				except:
					pass
			else:
				obj_hash_count[name] += 1

	for name in obj_hash_count:
		print name + " (" + obj_hash_filename[name] + "): " + str(obj_hash_count[name])

	#for x in gc.get_objects():
		#s = str(x)
		#if len(s) > 80:
			#s = s[:80]
		#print type(x),"\n  ", s

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
	os.open(REDIRECT_TO, os.O_RDWR)
	os.dup2(0, 1)
	os.dup2(0, 2)

	return(0)

def load_module(client_data):
	try:
		client_module = sys.modules[client_data]
		if not isinstance(client_module, types.ModuleType):
			raise KeyError
	except KeyError:
		try:
			client_module = __import__(client_data, globals(), locals(), [''])
		except:
			print "Given module not found: " + client_data
			return None

		sys.modules[client_data] = client_module

	## try to find the main class
	try:
		client_class = getattr(client_module, client_data)
	except:
		print "Module does not provide start function: " + client_data
		return None

	return client_class

if __name__ == "__main__":

	myargv = sys.argv[1:]

	if (len(myargv) > 0) and (myargv[0] == "-f"):
		myargv.pop(0)
	else:
		createDaemon()

	## for the applications
	import etk
	import edje
	import ecore.evas

	first_run = 1
	running_threads = {}

	if os.path.exists(UNIX_PATH):
		os.remove(UNIX_PATH)

	server = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
	server.bind(UNIX_PATH)

	## preload exposure
	load_module("exposure")

	if not (gc.isenabled()):
		print "enabling gc"
		gc.enable()

	#print "trace leaking objects"
	#gc.set_debug(gc.DEBUG_LEAK)

	while True:
		if (first_run) and (len(myargv) > 0):
			client_data = myargv[0]
		else:
			try:
				client_data = server.recv(1024)
			except socket.timeout:
				continue
			except socket.error, msg:
				print "Exiting daemon: " + msg[1]
				break

		first_run = 0

		if (client_data == "exit_daemon"):
			break

		if (client_data == "memory_trace"):
			print_mem_leaks()
			continue

		client_class = load_module(client_data)

		if (client_class == None):
			continue

		## make the window the top window if the module is already running
		if (client_data in running_threads) and running_threads[client_data].isAlive():
			print "focussing"
			running_threads[client_data].focus()
		else:
			## load and start the module
			#print "create new thread"
			running_threads[client_data] = client_class(etk, edje, ecore)
			running_threads[client_data].setName(client_data)
			running_threads[client_data].start()
			#print "started"

		#running_threads[client_data] = client_class(etk, edje, ecore)
		#profile.run('running_threads[client_data].run()', 'out_file.prof')

	## close all threads
	for mythread in running_threads:
		if (running_threads[mythread].isAlive()):
			running_threads[mythread].stop()
			running_threads[mythread].join()

	server.close()

