#ifndef PHONEGUI_INIT_H
#define PHONEGUI_INIT_H

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include "pipe.h"

typedef enum {
    MODE_SIM_AUTH,
    MODE_INCOMING_CALL,
    MODE_OUTGOING_CALL
} PhoneguiMode;

Ecore_Evas   *ee;
Evas         *evas;
Evas_Object  *edje;
double       edje_w, edje_h;

PhoneguiMode phonegui_mode;
PipeHandler pipe_handler;

#endif
