DESCRIPTION = "frameworkd EFL phonegui"
SECTION = "openmoko/pim"
PKG_TAGS_${PN} = "group::communication"
DEPENDS += " dbus-glib libframeworkd-glib etk evas ecore edje"
PV = "0.0.1+${PR}-svnr${SRCREV}"
PR = "r7"

inherit shr pkgconfig autotools

do_stage () {
        oe_libinstall -so libframeworkd-phonegui-efl ${STAGING_LIBDIR}

        autotools_stage_includes
}
