AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall @DIALER_CFLAGS@

AM_LDFLAGS = @DIALER_LIBS@

bin_PROGRAMS = ophonekitd

ophonekitd_SOURCES = \
	ophonekitd-main.c ophonekitd-main.h

ophonekitd_LDADD = \
  @DIALER_LIBS@ \
  @JANA_LIBS@ \
  @NOTIFY_LIBS@ \
  @ALSA_LIBS@ \
  libframeworkd-glib.la \
  libphonegui.la \

