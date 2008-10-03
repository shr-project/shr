localedir = $(datadir)/locale
AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall @PHONEGUI_CFLAGS@

lib_LTLIBRARIES = libframeworkd-phonegui-efl.la

libframeworkd_phonegui_efl_la_SOURCES = \
	phonegui-dialer.h phonegui-init.h phonegui-messages.c phonegui-outgoing-call.h pipe.c \
	phonegui-incoming-call.h phonegui-message.c phonegui-messages.h phonegui-sim-auth.c pipe.h \
	phonegui-dialer.c phonegui-init.c phonegui-message.h phonegui-outgoing-call.c phonegui-sim-auth.h

libframeworkd_phonegui_efl_HEADERS = 


libframeworkd_phonegui_efldir = $(includedir)/frameworkd-phonegui-efl

libframeworkd_phonegui_efl_la_LDFLAGS = $(all_libraries) -version-info 0:0:0

libframeworkd_phonegui_efl_la_LIBADD = @PHONEGUI_LIBS@ 
