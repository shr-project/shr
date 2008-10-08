DESCRIPTION = "Ophonekitd daemon"
SECTION = "openmoko/pim"
PKG_TAGS_${PN} = "group::communication"
DEPENDS += " dbus-glib libframeworkd-glib"
PV = "0.0.1-shr+${PR}+svnr${SVNREV}"
PR = "r10"
PE = "1"

inherit shr

do_install_append() {
        install -d ${D}${sysconfdir}/X11/Xsession.d/
        install -m 0755 ${WORKDIR}/80ophonekitd ${D}${sysconfdir}/X11/Xsession.d/
}

FILES_${PN} += "${datadir} ${sysconfdir}"

