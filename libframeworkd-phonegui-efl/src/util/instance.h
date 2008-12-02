#ifndef _INSTANCE_MANAGER_H
#define _INSTANCE_MANAGER_H

#include "window.h"

enum InstanceType {
    INSTANCE_INCOMING_MESSAGE,
    INSTANCE_CALL
};

void instance_manager_add(int type, void *id, struct Window *win);
struct Window *instance_manager_remove(int type, int id);
struct Window *instance_manager_remove_by_ecore_evas(Ecore_Evas *ee);

#endif



