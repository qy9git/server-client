#include <fcntl.h>
#include <unistd.h>
#include <sodium.h>
#define N 10
struct client{
    unsigned char ckey[crypto_auth_KEYBYTES];
    unsigned char skey[crypto_auth_KEYBYTES];
};
int main(void){
    struct client A[N];
    for(int i=0;i<N;++i){
        crypto_auth_keygen(A[i].ckey);
        crypto_auth_keygen(A[i].skey);
    }
    write(creat("./clientlist",0600),A,sizeof A);
    write(creat("./clientinfo",0600),A,sizeof*A);
    return 0;
}
