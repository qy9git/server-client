#ifndef COMMON_H
#define COMMON_H

//feature macros
#define _GNU_SOURCE

//headers
#include <string.h>
#include <assert.h>

#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <time.h>

#include <sodium.h>

//debug
#include "logger.h"
#ifdef NDEBUG
#define test(x) if(x)
#else
#define test(x) if(0)
#endif

//constants
#define S_PORT 8076u
#define auth_bytes 17 //in bytes

//types
struct usr{
    unsigned long long COUNT;
    unsigned char ckey[crypto_auth_KEYBYTES];
    unsigned char skey[crypto_auth_KEYBYTES];
};

#endif
