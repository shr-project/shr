AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall @DIALER_CFLAGS@

AM_LDFLAGS = @DIALER_LIBS@

INCLUDES = -I$(top_srcdir)/src/libphonegui

dialer-marshal.h: dialer-marshal.list
	( @GLIB_GENMARSHAL@ $< --header > dialer-marshal.tmp \
	&& mv dialer-marshal.tmp dialer-marshal.h ) \
	|| ( rm -f dialer-marshal.tmp && exit 1 )

dialer-marshal.c: dialer-marshal.list dialer-marshal.h
	( @GLIB_GENMARSHAL@ $< --body > dialer-marshal.tmp \
	&& mv dialer-marshal.tmp dialer-marshal.c ) \
	|| ( rm -f dialer-marshal.tmp && exit 1 )

BUILT_SOURCES= dialer-marshal.c dialer-marshal.h

noinst_LIBRARIES = libdialer-common.a

libdialer_common_a_SOURCES = \
	dbus.c dbus.h sim.c sim.h network.c network.h call.c call.h device.c device.h sms.h sms.c $(BUILT_SOURCES)

bin_PROGRAMS = ophonekitd

ophonekitd_SOURCES = \
	ophonekitd-main.c ophonekitd-main.h

ophonekitd_LDADD = $(top_srcdir)/src/libphonegui/libphonegui.a libdialer-common.a @DIALER_LIBS@ @JANA_LIBS@ @NOTIFY_LIBS@ @ALSA_LIBS@ 
