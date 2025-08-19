#include <fcntl.h>
#include <unistd.h>
#include <sodium.h>
#define N 10
struct usr{
    unsigned char ckey[crypto_auth_KEYBYTES];
    unsigned char skey[crypto_auth_KEYBYTES];
};
int main(void){
    struct usr A[N];
    for(int i=0;i<N;++i){
        crypto_auth_keygen(A[i].ckey);
        crypto_auth_keygen(A[i].skey);
    }
    write(creat("./usrlist",0600),A,sizeof A);
    write(creat("./usrinfo",0600),A,sizeof*A);
    return 0;
}
