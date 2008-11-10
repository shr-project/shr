#ifndef _PHONEGUI_DIALOG_H
#define _PHONEGUI_DIALOG_H

#include <Ecore_Evas.h>

void phonegui_dialog_show(int type);
void phonegui_dialog_hide();

void dialog_delete(Ecore_Evas *ee);
void dialog_event(int event);

void dialog_button_close_clicked();

void dialog_main_show();
void dialog_main_hide();

#endif

