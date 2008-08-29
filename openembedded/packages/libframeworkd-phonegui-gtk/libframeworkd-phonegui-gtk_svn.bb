DESCRIPTION = "The Openmoko Dialer"
SECTION = "openmoko/pim"
PKG_TAGS_${PN} = "group::communication"
DEPENDS += " libmokoui2 libmokojournal2 pulseaudio dbus-glib libnotify libjana libframeworkd-glib libhito"
PV = "0.0.1-shr"
PR = "r1"
PE = "1"

inherit shr pkgconfig autotools

do_stage () {
        oe_libinstall -so libframeworkd-phonegui-gtk ${STAGING_LIBDIR}

        autotools_stage_includes
}
