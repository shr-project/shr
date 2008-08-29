DESCRIPTION = "The Openmoko Messages"
SECTION = "openmoko/pim"
PKG_TAGS_${PN} = "group::communication"
DEPENDS += " libmokoui2 libmokojournal2 dbus-glib libnotify libjana libframeworkd-glib"
PV = "0.0.1-shr"
PR = "r8"
PE = "1"

inherit shr pkgconfig autotools
