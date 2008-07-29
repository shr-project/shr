AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall @DIALER_CFLAGS@

AM_LDFLAGS = @DIALER_LIBS@

dialer-marshal.h: dialer-marshal.list
	( @GLIB_GENMARSHAL@ $< --header > dialer-marshal.tmp \
	&& mv dialer-marshal.tmp dialer-marshal.h ) \
	|| ( rm -f dialer-marshal.tmp && exit 1 )

dialer-marshal.c: dialer-marshal.list dialer-marshal.h
	( @GLIB_GENMARSHAL@ $< --body > dialer-marshal.tmp \
	&& mv dialer-marshal.tmp dialer-marshal.c ) \
	|| ( rm -f dialer-marshal.tmp && exit 1 )

BUILT_SOURCES= dialer-marshal.c dialer-marshal.h

lib_LTLIBRARIES = libframeworkd-glib.la

libframeworkd_glib_la_SOURCES = \
	frameworkd-glib-dbus.c frameworkd-glib-dbus.h \
  frameworkd-glib-sim.c frameworkd-glib-sim.h \
  frameworkd-glib-network.c frameworkd-glib-network.h \
  frameworkd-glib-call.c frameworkd-glib-call.h \
  frameworkd-glib-device.c frameworkd-glib-device.h \
  frameworkd-glib-sms.h frameworkd-glib-sms.c \
  $(BUILT_SOURCES)

libframeworkd_glib_HEADERS = frameworkd-glib-dbus.h \
  				  frameworkd-glib-sim.h \
  				  frameworkd-glib-network.h \
				  frameworkd-glib-call.h \
				  frameworkd-glib-device.h \
				  frameworkd-glib-phonegui.h \
				  frameworkd-glib-sms.h

libframeworkd_glibdir = $(includedir)/frameworkd-glib

libframeworkd_glib_la_LDFLAGS = $(all_libraries) -version-info 0:0:0 -no-undefined

