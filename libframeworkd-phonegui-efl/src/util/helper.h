#ifndef _HELPER_H
#define _HELPER_H

#include <time.h>
#include <glib.h>

time_t time_stringtotimestamp(const char *str);
void string_strip_html(char *string);
gboolean string_is_number(const char *string);

#endif
