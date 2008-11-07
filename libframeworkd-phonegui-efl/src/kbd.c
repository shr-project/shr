#include "kbd.h"
#include <Ecore.h>
#include "phonegui-init.h"

extern Ecore_Evas *ee;

void kbd_show(KeyboardMode mode) {
    ecore_x_e_virtual_keyboard_state_set(
        ecore_evas_software_x11_window_get(ecore_evas_ecore_evas_get(evas_object_evas_get(win))),
        mode
    );
}

void kbd_hide() {
    ecore_x_e_virtual_keyboard_state_set(
        ecore_evas_software_x11_window_get(ecore_evas_ecore_evas_get(evas_object_evas_get(win))),
        KEYBOARD_OFF
    );
}

