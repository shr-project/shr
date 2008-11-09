DESCRIPTION = "Standard icon theme for the SHR distribution"
SECTION = "openmoko/base"
PKG_TAGS_${PN} = "group::communication"
PV = "0.0.1+${PR}-svnr${SRCREV}"
PR = "r0"

inherit shr autotools

# S = "${WORKDIR}/icons"

pkg_postinst_shr-theme () {
}

PACKAGE_ARCH = "all"

