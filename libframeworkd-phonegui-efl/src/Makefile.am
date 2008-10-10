AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall @PHONEGUI_CFLAGS@
AM_LDFLAGS = $(all_libraries) -lpthread @PHONEGUI_LIBS@ @GLIB_LIBS@ @EVAS_LIBS@ @ECORE_EVAS_LIBS@ @EDJE_LIBS@ @ECORE_LIBS@ @ETK_LIBS@

lib_LTLIBRARIES = libframeworkd-phonegui-efl.la

libframeworkd_phonegui_efl_la_LDFLAGS = $(all_libraries) -version-info 0:0:0 -no-undefined

libframeworkd_phonegui_efl_la_SOURCES = \
	phonegui-dialer.h phonegui-init.h phonegui-messages.c phonegui-outgoing-call.h pipe.c \
	phonegui-incoming-call.c phonegui-incoming-call.h phonegui-message.c phonegui-messages.h phonegui-sim-auth.c pipe.h \
	phonegui-dialer.c phonegui-init.c phonegui-message.h phonegui-outgoing-call.c phonegui-sim-auth.h \
	phonegui-contacts.h phonegui-contacts.c \
	etk_tree_model_edje.h etk_tree_model_edje.c

libframeworkd_phonegui_efl_HEADERS = 


libframeworkd_phonegui_efldir = $(includedir)/frameworkd-phonegui-efl
