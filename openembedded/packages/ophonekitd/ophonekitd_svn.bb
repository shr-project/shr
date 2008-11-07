DESCRIPTION = "Ophonekitd daemon"
SECTION = "openmoko/pim"
PKG_TAGS_${PN} = "group::communication"
DEPENDS += " dbus-glib libframeworkd-glib libframeworkd-phonegui"
PV = "0.0.1+${PR}-svnr${SRCREV}"
PR = "r7"

inherit shr

SRC_URI += " file://80ophonekitd"

do_install_append() {
        install -d ${D}${sysconfdir}/X11/Xsession.d/
        install -m 0755 ${WORKDIR}/80ophonekitd ${D}${sysconfdir}/X11/Xsession.d/
}

FILES_${PN} += "${datadir} ${sysconfdir}"

