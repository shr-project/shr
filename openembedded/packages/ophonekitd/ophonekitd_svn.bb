DESCRIPTION = "Ophonekitd daemon"
SECTION = "openmoko/pim"
PKG_TAGS_${PN} = "group::communication"
DEPENDS += " dbus-glib libframeworkd-glib"
PV = "0.0.1-shr"
PR = "r8"
PE = "1"

inherit shr