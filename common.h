#ifndef COMMON_H
#define COMMON_H

//feature macros
#define _GNU_SOURCE
//TODO investigate feature test macros

//headers
#include <string.h> // IWYU pragma: export
#include <assert.h>
#include <time.h> // IWYU pragma: export
#include <unistd.h>
#include <signal.h> // IWYU pragma: export
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <sodium.h>

//debug
#include "logger.h" // IWYU pragma: export

//constants
#define S_PORT 8076u
#define auth_nonce_bytes 64u //in bytes

//types
struct usr{
    unsigned char ckey[crypto_auth_hmacsha512_KEYBYTES];
    unsigned char skey[crypto_auth_hmacsha512_KEYBYTES];
};

#endif
