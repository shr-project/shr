DESCRIPTION = "A Contact View for GTK+"
SECTION = "openmoko/pim"
PKG_TAGS_${PN} = "group::dsunication"
DEPENDS += " glib-2.0 gtk+ eds-dbus"
# eds-dbus provides libebook
PV = "0.6.0+svnr${SRCDATE}"
PR = "r1"
PE = "1"

inherit shr pkgconfig autotools

do_stage () {
        oe_libinstall -so libhito ${STAGING_LIBDIR}

	autotools_stage_includes
}

