#include "instance.h"
#include <dbus/dbus-glib.h>
#include <Ecore_Evas.h>

struct Instance {
    int type;
    void *id;
    struct Window *win;
};


static int instances_size = 0;
static struct Instance *instances;


void instance_manager_add(int type, void *id, struct Window *win) {
    g_debug("instance_manager_add: type=%d, id=%d, window=%d", type, id, win);
    instances_size++;
    if(instances_size == 1) {
        instances = malloc(sizeof(struct Instance));
        g_debug("malloc'ed");
    } else {
        instances = realloc(instances, sizeof(struct Instance)*instances_size);
        g_debug("realloc'ed");
    }

    g_debug("add window: %d", win->win);

    instances[instances_size-1].type = type;
    instances[instances_size-1].id = id;
    instances[instances_size-1].win = win;

    g_debug("done");
}

struct Window *instance_manager_remove(int type, int id) {
    int i;
    for(i = 0 ; i < instances_size ; i++) {
        if(instances[i].type == type && instances[i].id == id) {
            instances[i].type = -1;
            g_debug("instance_manager_remove: %d", instances[i].win);
            return instances[i].win;
        }
    }

    // TODO: Free things

    return -1;
}


struct Window *instance_manager_remove_by_ecore_evas(Ecore_Evas *ee) {
    int i;
    for(i = 0 ; i < instances_size ; i++) {
        g_debug("foreach win: %d", instances[i].win->win);
        if(ee == ecore_evas_ecore_evas_get(evas_object_evas_get(instances[i].win->win))) {
            instances[i].type = -1;
            g_debug("instance_manager_remove_by_ecore_evas: %d", instances[i].win);
            return instances[i].win;
        }
    }

    // TODO: Free things

    return -1;
}




