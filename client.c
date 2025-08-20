// Client in C
#include "common.h"
#include <sys/poll.h>
#include <unistd.h>

#define connection_retry_delay 20 //in seconds (unsigned int)
#define timeout_for_initial_connection 10000 //in miliseconds (int)
int main(void){
    log_init("./logs");
    logi("Starting client");

    struct usr self;
    {
        int usrinfo_fd=open("./usrinfo",O_RDONLY,0);
        if(usrinfo_fd<0)
            exp("failed to open usrinfo");
        if(read(usrinfo_fd,&self,sizeof self)!=sizeof self)
            exp("failed while reading usrinfo");
        logd("loaded userinfo");
        close(usrinfo_fd);
    }

    if(sodium_init()<0) // https://doc.libsodium.org/quickstart
        ex("failed to initialise the cryptography library libsodium");
    logd("Initialised libsodium");

    int cfd = socket(AF_INET6, SOCK_STREAM, 0); // man ipv6 && man 2 socket
    if(cfd < 0)
        exp("failed to create server socket");
    logd("IPV6 socket created");

    const int flags=fcntl(cfd,F_GETFD)|IPV6_V6ONLY; //man 3 fcntl
    if(flags<0)
        exp("failed to get socket flags");
    logd("fetched socket flags");

    reconnect:

    if(fcntl(cfd,F_SETFD,flags)) // man ipv6 && man 3 fcntl
        logrp("failed to set socket flags");
    else
        logd("Socket is made IPV6 only");

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
            sleep(connection_retry_delay);
        }
        logd("Connected to server");
    }

    if(fcntl(cfd,F_SETFD,flags|O_NONBLOCK)) // man ipv6 && man 3 fcntl
        logcp("failed to set socket flag");
    else
        logd("Socket is made IPV6 only");

    {
        struct pollfd S={cfd,POLLOUT|POLLIN,0};
        unsigned char server_nonce[auth_nonce_bytes];
        int read_progress=0;
        int write_progress=0;
        unsigned char client_nonce[auth_nonce_bytes];
        //TODO get the current time to be sure connecting doesn't last forever
        for(;;){//loop for the initial connection
            if(poll(&S,1,timeout_for_initial_connection)<0)
                logcp("poll error");
        }
    }

}
