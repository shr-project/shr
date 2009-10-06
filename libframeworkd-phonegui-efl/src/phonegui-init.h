#ifndef PHONEGUI_INIT_H
#define PHONEGUI_INIT_H

int phonegui_argc;
char **phonegui_argv;
void (*phonegui_exit_cb) ();

void phonegui_backend_init(int argc, char **argv, void (*exit_cb) ());

#endif
