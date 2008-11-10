#include "time.h"
#include <time.h>
#include <locale.h>

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

