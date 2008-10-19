DESCRIPTION = "A Contacts application"
SECTION = "openmoko/pim"
PKG_TAGS_${PN} = "group::communication"
DEPENDS += "dbus-glib libframeworkd-glib libframeworkd-phonegui"
PV = "0.0.1+${PR}-svnr${SRCREV}"
PR = "r3"

inherit shr pkgconfig autotools
