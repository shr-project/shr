DESCRIPTION = "The Openmoko Messages"
SECTION = "openmoko/pim"
PKG_TAGS_${PN} = "group::communication"
DEPENDS += "dbus-glib libframeworkd-glib libframeworkd-phonegui"
PV = "0.0.1-shr+svnr${SRCREV}"
PR = "r12"
PE = "1"

inherit shr pkgconfig autotools
