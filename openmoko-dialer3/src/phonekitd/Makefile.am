AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall @DIALER_CFLAGS@

AM_LDFLAGS = @DIALER_LIBS@

noinst_LIBRARIES = libdialer-common.a

libdialer_common_a_SOURCES = \
	dbus.c dbus.h sim.c sim.h network.c network.h call.c call.h device.c device.h 

bin_PROGRAMS = ophonekitd

ophonekitd_SOURCES = \
	ophonekitd-main.c ophonekitd-main.h

ophonekitd_LDADD = libdialer-common.a @DIALER_LIBS@ @JANA_LIBS@ @NOTIFY_LIBS@ @ALSA_LIBS@ 
