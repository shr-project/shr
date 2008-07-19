DESCRIPTION = "The Openmoko Dialer"
SECTION = "openmoko/pim"
PKG_TAGS_${PN} = "group::communication"
DEPENDS += " libmokoui2 libmokojournal2 pulseaudio dbus-glib"
PV = "0.0.1+svnr${SRCDATE}"
PR = "r8"
PE = "1"


inherit shr


