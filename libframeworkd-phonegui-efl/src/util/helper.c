#define _XOPEN_SOURCE /* glibc2 needs this for strptime */
#include "time.h"
#include <time.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

time_t time_stringtotimestamp(const char *str) {
    time_t ts = 0;
    struct tm date;
    // Set locale
    setlocale(LC_TIME, "C");

    // Parse date string, for example: Sun Sep 28 23:20:24 2008 +0200
    if(strptime(str, "%a %h %e %T %Y %z", &date) != NULL) {
        // Generate long from struct tm
        ts = mktime(&date);
    }
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


void string_replace_newline(char *string) {
    char *in_p = string, *out_p = string;
    for (; *in_p; in_p++, out_p++) {
        switch (*in_p) {
            case '\n':
            case '\t':
                *out_p = ' ';
                break;
            default:
                *out_p = *in_p;
                break;
        }
    }
    *out_p = '\0';
}



char *string_replace_with_tags(char *string)
{
    int newlen = 0;
    char *in_p = string, *out_p;

    /* scan the string to see how much longer we have to get */
    while (*in_p) {
        switch (*in_p++) {
            case '\n':    newlen += 3; break;
            case '\t':    newlen += 4; break;
            case '<':
	    case '>':	  newlen += 3; break;
            case '\t':    newlen += 4; break;
        }
    }

    if (!newlen)
       return (string);

    newlen += strlen(string);
    char *newstring = malloc(newlen+1);
    in_p = string;
    out_p = newstring;
    while (*in_p) {
        switch (*in_p) {
            case '\n':
                *out_p++ = '<';
                *out_p++ = 'b';
                *out_p++ = 'r';
                *out_p++ = '>';
                break;
            case '\t':
                *out_p++ = '<';
                *out_p++ = 't';
                *out_p++ = 'a';
                *out_p++ = 'b';
                *out_p++ = '>';
                break;
            case '<':
                *out_p++ = '&';
                *out_p++ = 'l';
                *out_p++ = 't';
                *out_p++ = ';';
                break;
            case '>':
                *out_p++ = '&';
                *out_p++ = 'g';
                *out_p++ = 't';
                *out_p++ = ';';
                break;
            case '&':
                *out_p++ = '&';
                *out_p++ = 'a';
                *out_p++ = 'm';
                *out_p++ = 'p';
                *out_p++ = ';';
                break;
            default:
                *out_p++ = *in_p;
                break;
        }
        in_p++;
    }
    *out_p = '\0';

    return (newstring);
}


gboolean string_is_number(const char *string) {
    if(!strlen(string)) {
        return FALSE;
    }

    const char *p = string;
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
    if(strlen(string) < 4 || strlen(string) > 8)
        return FALSE;

    const char *p = string;
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

    const char *p = string;
    while(*p) {
        if(*p < '0' || *p > '9')
            return FALSE;
        *p++;
    }

    return TRUE;
}


