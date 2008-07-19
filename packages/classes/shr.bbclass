#class for all the sources under shr project

inherit autotools pkgconfig openmoko2

HOMEPAGE = "http://projects.openmoko.org/projects/shr/"
SHR_RELEASE ?= "shr"
SHR_MIRROR ?= "svn://svn.projects.openmoko.org/svnroot/shr"

LICENSE = "${@openmoko_two_get_license(d)}"
SUBDIR = "${@openmoko_two_get_subdir(d)}"

SRC_URI := "${SHR_MIRROR}/trunk/;module=${PN};rev=HEAD;proto=http"
S = "${WORKDIR}/${PN}"

FILES_${PN} += "${datadir}/icons"

CVS_TARBALL_STASH =""
SVNREV = "r${SRCREV}"
#SVNREV = "${SRCDATE}"
