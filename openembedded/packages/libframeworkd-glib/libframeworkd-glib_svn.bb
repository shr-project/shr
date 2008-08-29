DESCRIPTION = "Frameworkd / GLib bindings"
SECTION = "openmoko/pim"
PKG_TAGS_${PN} = "group::communication"
DEPENDS += " dbus-glib"
PV = "0.0.1-shr"
PR = "r8"
PE = "1"

inherit shr autotools pkgconfig

do_stage () {
        oe_libinstall -so libframeworkd-glib ${STAGING_LIBDIR}

        autotools_stage_includes
}
