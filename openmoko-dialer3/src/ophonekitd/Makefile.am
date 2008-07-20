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

noinst_LIBRARIES = libphonekit-common.a

libphonekit_common_a_SOURCES = \
	dbus.c dbus.h \
  sim.c sim.h \
  network.c network.h \
  call.c call.h \
  device.c device.h \
  sms.h sms.c \
  $(BUILT_SOURCES)

# libphonekit_common_a_LIBADD = $(top_srcdir)/src/libphonegui/libphonegui.a

bin_PROGRAMS = ophonekitd

ophonekitd_SOURCES = \
	ophonekitd-main.c ophonekitd-main.h

ophonekitd_LDADD = \
  @DIALER_LIBS@ \
  @JANA_LIBS@ \
  @NOTIFY_LIBS@ \
  @ALSA_LIBS@ \
  libphonekit-common.a \
  $(top_srcdir)/src/libphonegui/libphonegui.a \
  $(top_srcdir)/src/common/libdialer-common.a
