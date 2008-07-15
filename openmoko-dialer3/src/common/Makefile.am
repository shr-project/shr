AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall @DIALER_CFLAGS@

AM_LDFLAGS = @DIALER_LIBS@

noinst_LIBRARIES = libdialer-common.a

libdialer_common_a_SOURCES = \
	dbus.c dbus.h
