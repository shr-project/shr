SECTION = "openmoko/panel-plugin"
DEPENDS += "matchbox-panel-2 libmokopanelui2"

inherit shr

FILES_${PN} = "${libdir}/matchbox-panel/lib*.so* ${datadir}"
