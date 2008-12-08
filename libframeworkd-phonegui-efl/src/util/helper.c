#include "time.h"
#include <time.h>
#include <locale.h>
#include <glib.h>

time_t time_stringtotimestamp(const char *str) {
    // Set locale
    setlocale(LC_TIME, "C");

    // Parse date string, for example: Sun Sep 28 23:20:24 2008 +0200
    struct tm date;
    if(strptime(str, "%a %h %e %T %Y %z", &date) == NULL) {
        return NULL;
    } 

    // Generate long from struct tm
    time_t ts = mktime(&date);

    return ts;
}


void string_strip_html(char *string) {
    char *in_p = string, *out_p = string;
    while (*in_p) {
        if (*in_p == '<')
            while (*in_p && *in_p++ != '>');
        else
          *out_p++ = *in_p++;
    }
    *out_p = '\0';
}


gboolean string_is_number(const char *string) {
    if(!strlen(string)) {
        return FALSE;
    }

    char *p = string;
    while(*p && *p == '+')
        *p++;

    while(*p) {
        if(*p < '0' || *p > '9')
            return FALSE;
        *p++;
    }

    return TRUE;
}

gboolean string_is_pin(const char *string) {
    if(strlen(string) != 4)
        return FALSE;

    char *p = string;
    while(*p) {
        if(*p < '0' || *p > '9')
            return FALSE;
        *p++;
    }

    return TRUE;
}


gboolean string_is_puk(const char *string) {
    if(strlen(string) != 8)
        return FALSE;

    char *p = string;
    while(*p) {
        if(*p < '0' || *p > '9')
            return FALSE;
        *p++;
    }

    return TRUE;
}


