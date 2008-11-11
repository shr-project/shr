#class for all the sources under shr project

inherit autotools pkgconfig openmoko2

HOMEPAGE = "http://projects.openmoko.org/projects/shr/"
SHR_RELEASE ?= "shr"
SHR_MIRROR ?= "svn://daria.forty-two.fr/shr"

LICENSE = "${@openmoko_two_get_license(d)}"
SUBDIR = "${@openmoko_two_get_subdir(d)}"

SRC_URI := "${SHR_MIRROR}/trunk/;module=${PN};proto=svn"
#SRC_URI := "file://${TOPDIR}/shr/${PN}"
S = "${WORKDIR}/${PN}"

FILES_${PN} += "${datadir}/icons"

CVS_TARBALL_STASH =""
SVNREV = "r${SRCREV}"
#SVNREV = "${SRCDATE}"
