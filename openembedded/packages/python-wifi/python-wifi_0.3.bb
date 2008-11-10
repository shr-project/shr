DESCRIPTION = "Fusil is a Python library used to write fuzzing programs."
HOMEPAGE = "http://fusil.hachoir.org/"
SECTION = "devel/python"
LICENSE = "GPLv2"
SRCNAME = "python-wifi"

SRC_URI = "http://pypi.python.org/packages/source/p/${SRCNAME}/${SRCNAME}-${PV}.linux-i686.tar.gz"
S = "${WORKDIR}/${SRCNAME}"

inherit distutils

RDEPENDS_${PN} = "\
  python-ctypes \
  python-datetime \
"



