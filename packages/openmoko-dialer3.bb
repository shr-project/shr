#Thanks to smurfy-phil
DESCRIPTION = "The Openmoko Dialer"
#
SECTION = "openmoko/applications"
#
PV = "0.0.1+svnr${SRCPV}"
#
 
#
SRC_URI = "svn://svn.projects.openmoko.org/svnroot/shr/trunk;module=openmoko-dialer2;rev=18;proto=http"
#
 
#
S = "${WORKDIR}/openmoko-dialer2/"
#
SVNREV = "r${SRCREV}"
#
 
#
inherit autotools pkgconfig
#
 
#
DEPENDS += " eds-dbus libgsmd"
