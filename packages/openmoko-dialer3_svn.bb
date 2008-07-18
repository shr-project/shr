DESCRIPTION = "The Openmoko Dialer"
SECTION = "openmoko/pim"
PKG_TAGS_${PN} = "group::communication"
DEPENDS += " eds-dbus libgsmd"
PV = "0.0.1+svnr${SRCDATE}"
PR = "r8"
PE = "1"


inherit shr


