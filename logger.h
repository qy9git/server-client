#ifndef LOGGER_H
#define LOGGER_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

void log_time(FILE *stream);

#ifdef LOGGING
#define logERR(...) fprintf(stderr,__VA_ARGS__);fputc('\n',stderr);fflush(stderr);
#define logpERR(...) fprintf(stderr,__VA_ARGS__);fputs(" | ",stderr);fputs(strerror(errno),stderr);fputc('\n',stderr);fflush(stderr);
#define logiERR(...) log_time(stderr);fputs("[INFO] ",stderr);logERR(__VA_ARGS__)
#define logdERR(...) log_time(stderr);fputs("[DEBUG] ",stderr);logERR(__VA_ARGS__)
#define logsERR(...) log_time(stderr);fputs("[SERVER] ",stderr);logERR(__VA_ARGS__)
#define logwERR(...) log_time(stderr);fputs("[WARNING] ",stderr);logERR(__VA_ARGS__)
#define logrERR(...) log_time(stderr);fputs("[ERROR] ",stderr);logERR(__VA_ARGS__)
#define logcERR(...) log_time(stderr);fputs("[CRITICAL] ",stderr);logERR(__VA_ARGS__)
#define logfERR(...) log_time(stderr);fputs("[FATAL] ",stderr);logERR(__VA_ARGS__)
#define logwpERR(...) log_time(stderr);fputs("[WARNING] ",stderr);logpERR(__VA_ARGS__)
#define logrpERR(...) log_time(stderr);fputs("[ERROR] ",stderr);logpERR(__VA_ARGS__)
#define logcpERR(...) log_time(stderr);fputs("[CRITICAL] ",stderr);logpERR(__VA_ARGS__)
#define logfpERR(...) log_time(stderr);fputs("[FATAL] ",stderr);logpERR(__VA_ARGS__)
#define log_init(name) if(freopen(name,"a",stderr) == NULL)exp("Failed to initialise logger");logs("Started logging");
#else
#define logiERR(...)
#define logdERR(...)
#define logsERR(...)
#define logwERR(...)
#define logrERR(...)
#define logcERR(...)
#define logfERR(...)
#define logwpERR(...)
#define logrpERR(...)
#define logcpERR(...)
#define logfpERR(...)
#define log_init(name)
#endif

#ifdef VERBOSE
#define logOUT(...) printf(__VA_ARGS__);puts("\033[0m");fflush(stdout);
#define logpOUT(...) printf(__VA_ARGS__);printf(" | ");printf(strerror(errno));puts("\033[0m");fflush(stdout);
#define logiOUT(...) printf("\033[38;5;40m");log_time(stdout);printf("[INFO] ");logOUT(__VA_ARGS__)
#define logdOUT(...) printf("\033[38;5;39m");log_time(stdout);printf("[DEBUG] ");logOUT(__VA_ARGS__)
#define logsOUT(...) printf("\033[1m");log_time(stdout);printf("[SERVER] ");logOUT(__VA_ARGS__)
#define logwOUT(...) printf("\033[1m\033[38;5;166m");log_time(stdout);printf("[WARNING] ");logOUT(__VA_ARGS__)
#define logrOUT(...) printf("\033[1m\033[38;5;197m");log_time(stdout);printf("[ERROR] ");logOUT(__VA_ARGS__)
#define logcOUT(...) printf("\033[1m\033[38;5;191m\033[48;5;52m");log_time(stdout);printf("[CRITICAL] ");logOUT(__VA_ARGS__)
#define logfOUT(...) printf("\033[1m\033[38;5;191m\033[48;5;52m");log_time(stdout);printf("[FATAL] ");logOUT(__VA_ARGS__)
#define logwpOUT(...) printf("\033[1m\033[38;5;166m");log_time(stdout);printf("[WARNING] ");logpOUT(__VA_ARGS__)
#define logrpOUT(...) printf("\033[1m\033[38;5;197m");log_time(stdout);printf("[ERROR] ");logpOUT(__VA_ARGS__)
#define logcpOUT(...) printf("\033[1m\033[38;5;191m\033[48;5;52m");log_time(stdout);printf("[CRITICAL] ");logpOUT(__VA_ARGS__)
#define logfpOUT(...) printf("\033[1m\033[38;5;191m\033[48;5;52m");log_time(stdout);printf("[FATAL] ");logpOUT(__VA_ARGS__)
#else
#define logiOUT(...)
#define logdOUT(...)
#define logsOUT(...)
#define logwOUT(...)
#define logrOUT(...)
#define logcOUT(...)
#define logfOUT(...)
#define logwpOUT(...)
#define logrpOUT(...)
#define logcpOUT(...)
#define logfpOUT(...)
#endif

#define logi(...) do{logiERR(__VA_ARGS__)logiOUT(__VA_ARGS__)}while(0)
#define logd(...) do{logdERR(__VA_ARGS__)logdOUT(__VA_ARGS__)}while(0)
#define logs(...) do{logsERR(__VA_ARGS__)logsOUT(__VA_ARGS__)}while(0)
#define logw(...) do{logwERR(__VA_ARGS__)logwOUT(__VA_ARGS__)}while(0)
#define logr(...) do{logrERR(__VA_ARGS__)logrOUT(__VA_ARGS__)}while(0)
#define logc(...) do{logcERR(__VA_ARGS__)logcOUT(__VA_ARGS__)}while(0)
#define logf(...) do{logfERR(__VA_ARGS__)logfOUT(__VA_ARGS__)}while(0)
#define logwp(...) do{logwpERR(__VA_ARGS__)logwpOUT(__VA_ARGS__)}while(0)
#define logrp(...) do{logrpERR(__VA_ARGS__)logrpOUT(__VA_ARGS__)}while(0)
#define logcp(...) do{logcpERR(__VA_ARGS__)logcpOUT(__VA_ARGS__)}while(0)
#define logfp(...) do{logfpERR(__VA_ARGS__)logfpOUT(__VA_ARGS__)}while(0)
#define exp(...) do{logfp(__VA_ARGS__);exit(EXIT_FAILURE);}while(0)
#define ex(...) do{logf(__VA_ARGS__);exit(EXIT_FAILURE);}while(0)

//https://en.wikipedia.org/wiki/ANSI_escape_code
#define NB "\033[38;5;208m"
#define NBi(x) NB x"\033[38;5;40m"
#define NBd(x) NB x"\033[38;5;39m"
#define NBs(x) NB x"\033[38;5;0m"
#define NBw(x) NB x"\033[38;5;166m"
#define NBr(x) NB x"\033[38;5;197m"
#define NBc(x) NB x"\033[38;5;191m"
#define NBf(x) NB x"\033[38;5;191m"

#endif
/* API documentation:
 *
 * NB CSI for numbers
 *
 * They have the same signature as printf
 *
 * log_init(fileName) //redirects stderr to fileName
 * ex   //exit                           seen as fatal crashes
 * exp  //exit with perror               seen as fatal crashes
 * MACRO: VERBOSE (stdout) LOGGING (stderr)
 *    logw  //log warning               for functions that are expected to never fail   |1 time failures during runtime
 *    logr  //log error                 for important functions expected to never fail  |1 time failures during runtime
 *    logc  //log critical              for functions that are essential to the program |RUNNING but not working
 *    logf  //log fatal                 for functions that are integral to the program  |CRASH
 *    logwp //log warning with perror
 *    logrp //log error with perror
 *    logcp //log critical with perror
 *    logfp //log fatal with perror
 *    MACRO: NDEBUG
 *        logi //log info               verbose information informing generally what is going on
 *        logd //log debug              debug information concerning the internal workings and debugging
 *        logs //log server/misc        Generic debugging and messaging
 */


// TODO make VERBOSE work with LOGGING
// TODO make printf coloured
// TODO make it thread safe
