MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall $(DIALER_CFLAGS) $(FRAMEWORKD_PHONEGUI_CFLAGS)
AM_LDFLAGS = -ldl $(DBUS_GLIB_LIBS) $(FRAMEWORKD_PHONEGUI_LIBS)
 			 
bin_PROGRAMS = shr-dialer

shr_dialer_SOURCES = dialer-main.c

shr_dialer_LDADD = @DIALER_LIBS@ -ldl
