// Client in C
#include "common.h"

#define connect_timeout 15

int main(void){
    log_init("./logs");
    logi("Starting client");

    if(sodium_init()<0) // https://doc.libsodium.org/quickstart
        ex("failed to initialise the cryptography library libsodium");
    logd("Initialised libsodium");

    int cfd = socket(AF_INET6, SOCK_STREAM, 0); // man ipv6 && man 2 socket
    if(cfd < 0)
        exp("failed to create server socket");
    logd("IPV6 socket created");

    {
        int flags=fcntl(cfd,F_GETFD); //man 3 fcntl
        if(flags<0)
            exp("failed to get socket flags");
        if(IPV6_V6ONLY ^ flags)
            if(fcntl(cfd,F_SETFD,flags|IPV6_V6ONLY)) // man ipv6 && man 3 fcntl
                exp("failed to set socket flag");
        logd("Socket is made IPV6 only");
    }
    {
        const struct sockaddr_in6 ADR={  // man ipv6
            .sin6_family=AF_INET6,       // AF_INET6                     sa_family_t
            .sin6_port=htons(S_PORT),    // port number                  in_port_t
            .sin6_flowinfo=0,            // IPv6 flow information        uint32_t
            .sin6_addr=in6addr_loopback, // IPv6 address                 struct in6_addr
            .sin6_scope_id=0             // Scope ID (new in Linux 2.4)  uint32_t
        };
        while(connect(cfd,(struct sockaddr*)&ADR,sizeof ADR)<0){// man 3 connect
            logrp("Failed to connect to the server");
            sleep(connect_timeout);
        }
        logd("Connected to server");
    }
}
