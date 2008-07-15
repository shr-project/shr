#Thanks to smurfy-phil
DESCRIPTION = "The Openmoko Dialer"
SECTION = "openmoko/applications"
SRCPV = "0.0.1+svnr${SRCPV}"
PV="0.0.1"
 
#SRC_URI = "file:///home/ainu/moko/shr/shr/trunk/openmoko-dialer3"
SRC_URI = "svn://svn.projects.openmoko.org/svnroot/shr/trunk;module=openmoko-dialer3;rev=HEAD;proto=http"
 
S = "${WORKDIR}/openmoko-dialer3/"
SVNREV = "r${SRCREV}"
 
inherit autotools pkgconfig
 
DEPENDS += " eds-dbus libgsmd"
