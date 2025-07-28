#ifndef COMMON_H
#define COMMON_H
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
#define BLOCK 5000u

#endif
