AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall $(DIALER_CFLAGS) $(FRAMEWORKD_GLIB_CFLAGS)
AM_LDFLAGS = -ldl $(DBUS_GLIB_LIBS) $(FRAMEWORKD_GLIB_LIBS)
 			 
bin_PROGRAMS = openmoko-dialer

openmoko_dialer_SOURCES = dialer-main.c dialer-main.h dialer-phonegui.c dialer-phonegui.h

openmoko_dialer_LDADD = @DIALER_LIBS@ -ldl
