DESCRIPTION = "Mofi"
SECTION = "openmoko/settings"
PKG_TAGS_${PN} = "group::communication"
RDEPENDS = "python-core python-misc python-lang python-subprocess python-threading python-pygtk python-pygobject wpa-supplicant"
PV = "0.02"
PR = "r1"
SRC_URI = "http://projects.openmoko.org/frs/download.php/271/mofi.${PV}.tar.gz"
S = "${WORKDIR}/${PN}.${PV}"

do_install() {
	./install.sh
}
