AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DPKGDATADIR=\"$(pkgdatadir)\"

AM_CFLAGS = -Wall $(JANA_CFLAGS)  \
				$(DBUS_GLIB_CFLAGS) \
				$(FRAMEWORKD_GLIB_CFLAGS) \
				$(FRAMEWORKD_PHONEGUI_CFLAGS) 

AM_LDFLAGS = $(JANA_LIBS) \
			$(DBUS_GLIB_LIBS) \
			$(FRAMEWORKD_GLIB_LIBS) \
 			$(FRAMEWORKD_PHONEGUI_LIBS) 
 			 

bin_PROGRAMS = ophonekitd

ophonekitd_SOURCES = \
	ophonekitd-main.c ophonekitd-main.h

