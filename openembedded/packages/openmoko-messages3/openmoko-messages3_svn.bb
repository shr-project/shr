DESCRIPTION = "The Openmoko Messages"
SECTION = "openmoko/pim"
PKG_TAGS_${PN} = "group::communication"
DEPENDS += "dbus-glib libframeworkd-glib"
PV = "0.0.1-shr+svnr${SRCREV}"
PR = "r9"
PE = "1"

inherit shr pkgconfig autotools
