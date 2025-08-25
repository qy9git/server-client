// Server made for Linux in C
#include "common.h"

#define tcp_timeout 20000u //in miliseconds (unsigned int)
#define backlog 10
#define user_max 3
#define poll_max user_max+1
#define packet_m 5000u //in bytes
#define packet_h 20u //in bytes
#define packet_max packet_m+packet_h //in bytes
#define TCP_DEFER_ACCEPT_val 20 //in seconds
#define usr_number 10
struct client{
    struct sockaddr_in6 addr;
    char priv;//-1 new 0 user 126 moderator 127 administrator
    int rs;
    int ws;
    char rb[packet_m];
    char wb[packet_m];
};

int main(void){
    log_init("./logs");
    logi("Starting server");

    struct usr known_users[usr_number];
    int usrlist_fd=open("./usrlist",O_RDONLY,0);
    if(usrlist_fd<0)
        exp("failed to open usrlist");
    if(read(usrlist_fd,known_users,usr_number*sizeof*known_users)!=usr_number*sizeof*known_users)
        exp("failed while reading usrlist");
    logd("loaded user list to memory");

    if(sodium_init()<0) // https://doc.libsodium.org/quickstart
        ex("failed to initialise the cryptography library libsodium");
    logd("Initialised libsodium");

    {
        struct sigaction sa={.sa_handler=SIG_IGN};
        if(sigaction(SIGPIPE,&sa,NULL))
            exp("Unable to ignore SIGPIPE");
    }

    int sfd=socket(AF_INET6, SOCK_STREAM|SOCK_NONBLOCK, 0); // man ipv6 && man 2 socket
    if(sfd<0)
        exp("failed to create server socket");
    logd("Non blocking IPV6 socket created");

    {
        int ipv6_only=1;
        if(setsockopt(sfd, IPPROTO_IPV6, IPV6_V6ONLY, &ipv6_only, sizeof ipv6_only)) //man ipv6
            logwp("failed to option set IPv6 only");
        else
            logd("set option IPv6 only");

        int sec = TCP_DEFER_ACCEPT_val;
        if(setsockopt(sfd, IPPROTO_TCP, TCP_DEFER_ACCEPT, &sec, sizeof sec))
            logcp("failed to activate TCP_DEFER_ACCEPT");
        else
            logd("Set option TCP_DEFER_ACCEPT");

        unsigned ms=tcp_timeout;
        if(setsockopt(sfd, IPPROTO_TCP, TCP_USER_TIMEOUT, &ms, sizeof ms)) //man tcp
            logcp("failed to set timeout on tcp");
        else
            logd("set timeout on tcp");
    }
    {
        struct sockaddr_in6 ADR={// man ipv6
            .sin6_family=AF_INET6,     // AF_INET6                     sa_family_t
            .sin6_port=htons(S_PORT),  // port number                  in_port_t
            .sin6_flowinfo=0,          // IPv6 flow information        uint32_t
            .sin6_addr=in6addr_any,    // IPv6 address                 struct in6_addr
            .sin6_scope_id=0           // Scope ID (new in Linux 2.4)  uint32_t
        };
        if(bind(sfd,(struct sockaddr*)&ADR,sizeof ADR)) // man 3 bind
            exp("Failed to bind socket");
        logd("Socket binded");
    }

    if(listen(sfd,backlog)) // man 3 listen
        exp("failed to listen on port S_PORT");
    logd("Started listening on port "NB"%u",S_PORT);

    logi("Server Functional");


    struct client usr[user_max];
    struct pollfd pol[poll_max]={0}; //man 2 poll
    for(nfds_t i=0;i<user_max;++i)
        pol[i].fd=-1;
    pol[user_max].fd=sfd;
    pol[user_max].events=POLLIN;
    //main loop
    for(;;){
        int poll_ret = poll(pol,poll_max,-1); //man 3 poll
        if(poll_ret<0)
            logcp("poll failed, poll should never fail");
        if(pol[user_max].revents & ~(POLLIN|POLLOUT)){
            logc("there's a failure on the server fd "NB"%x",pol[user_max].revents);
            goto leave_connect;
        }
        if(pol[user_max].revents & POLLIN){// accepting connections
            struct sockaddr_in6 addr;
            socklen_t addrlen=sizeof addr;
            int cfd=accept(sfd,(struct sockaddr*)&addr,&addrlen);
            if(cfd<0){
                logrp("Failed to accept connection");
                goto leave_connect;
            }

            //TODO Proceed with a similar auth to client.c

            logd("Accepted incoming connection");
            for(nfds_t i=0;i<user_max;++i)
                if(pol[i].fd<0){
                    pol[i].fd=cfd;
                    memcpy(usr+i,&addr,sizeof addr);
                    usr[i].priv=-1;
                    logd("Registered incoming connection");
                    goto leave_connect;
                }
            close(cfd);
            logc("reached max poll rate, closed incoming connection");
        }

        leave_connect:
        for(nfds_t i=0;i<user_max;++i){
            if(pol[i].revents & ~(POLLIN|POLLOUT)){
                logr("found "NBr("%x")" on a client revents, closing connection",pol[i].revents);
                //this should never happen
                //just disconnect the user
            }
            if(pol[i].revents & POLLIN)
                if(!usr[i].rs){
                    ssize_t ret=read(pol[i].fd,usr[i].rb,packet_max);
                    if(ret<0){
                        logc("failed to read buffer");
                        ret=0;
                    }
                    usr[i].rs=(int)ret;
                }
            if(pol[i].revents & POLLOUT){
                ssize_t ret=write(pol[i].fd,usr[i].wb,packet_max);
                if(ret<0){
                    logc("failed to read buffer");
                    ret=0;
                }
                usr[i].ws-=(int)ret;
            }
        }
        //NOTE afte being done with io you should work on task queue, so it always goes: task io task io task io ...
    }
}

#ifndef NDEBUG
#define range(a,b,c) a<=b && b<=c
static_assert(range(1024u,S_PORT,65535u), "Out of bound port value");
static_assert(range(1u,backlog,128u), "Out of bound port backlog");//man 2 listen
static_assert(range(1u,poll_max,1024u), "Out of bound value for poll_max");//file descriptor limits should be checked
#endif
