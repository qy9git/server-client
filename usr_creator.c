#include "common.h"
#define N 10
int main(void){
    struct usr A[N];
    for(int i=0;i<N;++i){
        crypto_auth_hmacsha512_keygen(A[i].ckey);
        crypto_auth_hmacsha512_keygen(A[i].skey);
    }
    write(creat("./usrlist",0600),A,sizeof A);
    write(creat("./usrinfo",0600),A,sizeof*A);
    return 0;
}
