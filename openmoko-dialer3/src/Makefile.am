AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS = -Wall @DIALER_CFLAGS@

bin_PROGRAMS = openmoko-dialer

openmoko_dialer_SOURCES = dialer-main.c dialer-main.h

openmoko_dialer_LDADD = @DIALER_LIBS@
