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

UNIX_PATH = "/tmp/app-launcherd.sock"
DAEMON = "app-launcherd.py"


if (len(sys.argv) <> 2):
	print "You need to provide a module to be loaded by the daemon !"
	sys.exit(1)

unix_socket_client = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)

try:
	unix_socket_client.connect(UNIX_PATH)
except:
	print "Can't connect to unix socket - starting daemon ..."
	unix_socket_client.close()
	os.popen(DAEMON + " " + sys.argv[1])
	sys.exit(0)

unix_socket_client.send(sys.argv[1])
unix_socket_client.close()
