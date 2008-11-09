DESCRIPTION = "Settings application based on om-settings"
DEPENDS = "python-etk python-edbus"
RDEPENDS = "python-etk python-re python-codecs python-edbus python-pyxdg"
SECTION = "openmoko/pim"
PKG_TAGS_${PN} = "group::communication"
PV = "0.0.1+${PR}-svnr${SRCREV}"
PR = "r0"

inherit shr setuptools
# FILES_${PN} += "${prefix}/share/* ${sysconfdir}/X11/Xsession.d/*"

