#class for all the sources under shr project

inherit autotools pkgconfig

HOMEPAGE = "http://projects.openmoko.org/projects/shr/"
SHR_RELEASE ?= "shr"
SHR_MIRROR ?= "svn://svn.projects.openmoko.org/svnroot/shr"

def openmoko_two_get_license(d):
    import bb
    openmoko, section = bb.data.getVar('SECTION', d, 1).split("/")
    return "LGPL GPL".split()[section != "libs"]

def openmoko_two_get_subdir(d):
    import bb
    openmoko, section = bb.data.getVar('SECTION', d, 1).split("/")
    if section == 'base': return ""
    elif section == 'libs': return "libraries"
    elif section in 'apps tools pim'.split(): return "applications"
    elif section == "panel-plugin": return "panel-plugins"
    elif section == "inputmethods": return "inputmethods"
    elif section == "daemons": return "daemons"
    elif section == "misc": return "misc"
    else: return section

LICENSE = "${@openmoko_two_get_license(d)}"
SUBDIR = "${@openmoko_two_get_subdir(d)}"

SRC_URI := "${SHR_MIRROR}/trunk/;module=${PN};rev=HEAD;proto=http"
S = "${WORKDIR}/${PN}"

FILES_${PN} += "${datadir}/icons"

CVS_TARBALL_STASH =""
SVNREV = "r${SRCREV}"
#SVNREV = "${SRCDATE}"
