#include "kbd.h"
#include <Ecore.h>
#include "phonegui-init.h"

extern Ecore_Evas *ee;

void kbd_show(KeyboardMode mode) {
    ecore_x_e_virtual_keyboard_state_set(
        ecore_evas_software_x11_window_get(ee),
        mode
    );
}

void kbd_hide() {
    ecore_x_e_virtual_keyboard_state_set(
        ecore_evas_software_x11_window_get(ee),
        KEYBOARD_OFF
    );
}

