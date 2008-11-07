#include "time.h"
#include <time.h>

time_t time_stringtotimestamp(const char *str) {
    // Parse date string, for example: Sun Sep 28 23:20:24 2008 +0200
    struct tm date;
    strptime(str, "%a %h %e %T %Y %z", &date); 
    
    // Generate long from struct tm
    time_t ts = mktime(&date);

    return ts;
}
