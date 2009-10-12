MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall @DBUS_CFLAGS@
AM_LDFLAGS = @DBUS_LIBS@
 			 
bin_PROGRAMS = shr-dialer

shr_dialer_SOURCES = dialer-main.c

