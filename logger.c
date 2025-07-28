#include <stdio.h>
#include <time.h>
void log_time(FILE *stream){
    time_t unix_time = time(NULL);
    struct tm tm=*localtime(&unix_time);
    fprintf(stream,"[%d-%02d-%02d %02d:%02d:%02d] ",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
}
