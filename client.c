// Client in C
#include "common.h"

#define connection_retry_delay 20u //in seconds (unsigned int)
#define auth_send_timeout 20//in seconds (time_t)
#define auth_recv_timeout 20//in seconds (time_t)
#define tcp_timeout 20000u //in miliseconds (unsigned int)

int main(void){
    log_init("./logs");
    logi("Starting client");

    struct usr self;
    {
        int usrinfo_fd=open("./usrinfo",O_RDONLY,0); //man 2 open
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

    {
        struct sigaction sa={.sa_handler=SIG_IGN}; //man 2 sigaction
        if(sigaction(SIGPIPE,&sa,NULL))
            exp("Unable to ignore SIGPIPE");
    }

    //NOTE during auth a partial read/write is considered as an error. Blocking IO is expected to deliver all data.
    reconnect:;//TODO this is ugly with the ;

    const int cfd=socket(AF_INET6, SOCK_STREAM, 0); //man ipv6 && man 2 socket
    if(cfd<0){
        logcp("failed to create server socket");
        goto reconnect;
    }
    logd("IPV6 socket created");

    {
        struct timeval rcv={auth_recv_timeout,0};//man struct timeval (also supports milisecond)
        if(setsockopt(cfd, SOL_SOCKET, SO_RCVTIMEO, &rcv, sizeof rcv)){ //man 7 socket
            logcp("failed to set timeout on recv");
            goto reconnect;
        }
        logd("set timeout on recv");

        struct timeval snd={auth_send_timeout,0};
        if(setsockopt(cfd, SOL_SOCKET, SO_SNDTIMEO, &snd, sizeof snd)){ //man 7 socket
            logcp("failed to set timeout on send");
            goto reconnect;
        }
        logd("set timeout on send");

        int ipv6_only=1;
        if(setsockopt(cfd, IPPROTO_IPV6, IPV6_V6ONLY, &ipv6_only, sizeof ipv6_only)) //man ipv6
            logwp("failed to option set IPv6 only");
        else
            logd("set option IPv6 only");

        int tcp_nodelay=1;
        if(setsockopt(cfd, IPPROTO_TCP, TCP_NODELAY, &tcp_nodelay, sizeof tcp_nodelay)) //man tcp
            logwp("failed to set option tcp nodelay");
        else
            logd("set option tcp nodelay");

        unsigned ms=tcp_timeout;
        if(setsockopt(cfd, IPPROTO_TCP, TCP_USER_TIMEOUT, &ms, sizeof ms)) //man tcp
            logwp("failed to set timeout on tcp");
        else
            logd("set timeout on tcp");
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
            logrp("write failure during auth");
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
