DESCRIPTION = "Ophonekitd daemon"
SECTION = "openmoko/pim"
PKG_TAGS_${PN} = "group::communication"
DEPENDS += " dbus-glib libframeworkd-glib"
PV = "0.0.1-shr+${PR}+svnr${SRCREV}"
PR = "r13"
PE = "1"

inherit shr

SRC_URI += " file://80ophonekitd"

do_install_append() {
        install -d ${D}${sysconfdir}/X11/Xsession.d/
        install -m 0755 ${WORKDIR}/80ophonekitd ${D}${sysconfdir}/X11/Xsession.d/
}

FILES_${PN} += "${datadir} ${sysconfdir}"

