// Client in C
#include "common.h"
#include "logger.h"

#define connection_retry_delay 20 //in seconds (unsigned int)
#define timeout_for_initial_connection 10000 //in miliseconds (int)
#define auth_send_timeout 20//in seconds
#define auth_recv_timeout 20//in seconds
#define tcp_timeout 20 //in seconds

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

    const int cfd = socket(AF_INET6, SOCK_STREAM, 0); // man ipv6 && man 2 socket
    if(cfd < 0)
        exp("failed to create server socket");
    logd("IPV6 socket created");

    const int flags=fcntl(cfd,F_GETFD)|IPV6_V6ONLY; //man 3 fcntl
    if(flags<0)
        exp("failed to get socket flags");
    logd("fetched socket flags");

    //NOTE during auth partial read/write is an error. Blocking IO is expected to transfer all data.
    reconnect:

    if(fcntl(cfd,F_SETFD,flags)) // man ipv6 && man 3 fcntl
        logrp("failed to set socket flags");
    else
        logd("Socket is made IPV6 only");

    {
        struct timeval rcv = {auth_recv_timeout,0};//man struct timeval
        if(setsockopt(cfd, SOL_SOCKET, SO_RCVTIMEO, &rcv, sizeof rcv))
            logcp("failed to set timer on recv");
        struct timeval snd = {auth_send_timeout,0};//it also supports miliseconds
        if(setsockopt(cfd, SOL_SOCKET, SO_SNDTIMEO, &snd, sizeof snd))
            logcp("failed to set timer on send");
        int val=1;//boolean
        if(setsockopt(cfd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof val))
            logwp("failed to set timer on tcp");
    }
    {
        struct sockaddr_in6 A={  // man ipv6
            .sin6_family=AF_INET6,       // AF_INET6                     sa_family_t
            .sin6_port=htons(S_PORT),    // port number                  in_port_t
            .sin6_flowinfo=0,            // IPv6 flow information        uint32_t
            .sin6_addr=in6addr_loopback, // IPv6 address                 struct in6_addr
            .sin6_scope_id=0             // Scope ID (new in Linux 2.4)  uint32_t
        };
        unsigned id=0;//id of the client
        while(sendto(cfd,&id,sizeof id,MSG_FASTOPEN,(struct sockaddr*)&A,sizeof A)!=sizeof id){
            logcp("Failed to connect to the server");// man 2 sendto
            sleep(connection_retry_delay);
        }
    }
    {
        unsigned char server_nonce[auth_nonce_bytes];
        if(recv(cfd,server_nonce,sizeof server_nonce,MSG_WAITALL)!=sizeof server_nonce){
            logrp("recv failure during auth");
            close(cfd);
            goto reconnect;
        }
        unsigned char macANDnonce[crypto_auth_hmacsha512_BYTES+auth_nonce_bytes];
        crypto_auth_hmacsha512(macANDnonce,server_nonce,sizeof server_nonce,self.ckey);
        randombytes_buf(macANDnonce+crypto_auth_hmacsha512_BYTES,auth_nonce_bytes);
        if(write(cfd,macANDnonce,sizeof macANDnonce)!=sizeof macANDnonce){
            logrp("recv failure during auth");
            close(cfd);
            goto reconnect;
        }
        unsigned char server_mac[crypto_auth_hmacsha512_BYTES];
        if(recv(cfd,server_nonce,sizeof server_nonce,MSG_WAITALL)!=sizeof server_mac){
            logrp("recv failure during auth");
            close(cfd);
            goto reconnect;
        }
        if(crypto_auth_hmacsha512256_verify(server_mac,macANDnonce+crypto_auth_hmacsha512_BYTES,auth_nonce_bytes,self.skey)){
            logrp("Server mac verification failure");
            close(cfd);
            goto reconnect;
        }
        logd("successfully connected and authentificated with the server");
    }
}
