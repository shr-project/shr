MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DPKGDATADIR=\"$(pkgdatadir)\" @E_CFLAGS@ @LIBFRAMEWORKD_GLIB_CFLAGS@ @GLIB_CFLAGS@ 
# AM_CFLAGS = -Wall @LIBFRAMEWORKD_GLIB_CFLAGS@ @E_CFLAGS@ -lpthread 
# AM_LDFLAGS = $(all_libraries) @LIBFRAMEWORKD_GLIB_LIBS@ @GLIB_LIBS@ @E_LIBS@ -lpthread

lib_LTLIBRARIES = libframeworkd-phonegui-efl.la

libframeworkd_phonegui_efl_la_LIBADD = @LIBFRAMEWORKD_GLIB_LIBS@ @GLIB_LIBS@ @E_LIBS@ -lpthread
libframeworkd_phonegui_efl_la_LDFLAGS = -version-info 0:0:0 -no-undefined

libframeworkd_phonegui_efl_la_SOURCES = \
	phonegui-init.h phonegui-init.c \
	phonegui-dialer.h phonegui-dialer.c \
	phonegui-messages.h phonegui-messages.c \
	phonegui-message.h phonegui-message.c \
	phonegui-sim-auth.h phonegui-sim-auth.c \
	phonegui-contacts.h phonegui-contacts.c \
	phonegui-call.h phonegui-call.c \
	phonegui-dialog.h phonegui-dialog.c \
	etk_tree_model_edje.h etk_tree_model_edje.c \
	pipe.h pipe.c \
	kbd.h kbd.c \
	frame.h frame.c \
	time-helper.h time-helper.c \
	elm_keypad.h elm_keypad.c

libframeworkd_phonegui_efl_HEADERS = 


libframeworkd_phonegui_efldir = $(includedir)/frameworkd-phonegui-efl
