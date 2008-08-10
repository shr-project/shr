localedir = $(datadir)/locale
AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall @PHONEGUI_CFLAGS@ @JANA_CFLAGS@

lib_LTLIBRARIES = libframeworkd-phonegui-gtk.la

libframeworkd_phonegui_gtk_la_SOURCES = \
    init.c \
	pin.c pin.h \
	history.c history.h \
	keypad.c keypad.h \
	tips.c tips.h \
	contacts.c contacts.h \
	dialer-panel.c dialer-panel.h \
	dialer-textview.c dialer-textview.h \
	digit-button.c digit-button.h \
	dialer-defines.h

libframeworkd_phonegui_gtk_HEADERS = frameworkd-phonegui.h

libframeworkd_phonegui_gtkdir = $(includedir)/frameworkd-phonegui

libframeworkd_phonegui_gtk_la_LDFLAGS = $(all_libraries) -version-info 0:0:0

libframeworkd_phonegui_gtk_la_LIBADD = @PHONEGUI_LIBS@ @JANA_LIBS@
