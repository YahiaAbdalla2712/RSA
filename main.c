#include <stdio.h>

typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

#define R 0x100000000ULL

uint32 extended_ecluid(uint32 m ,uint32 b){
    int64 a1 = 1, a2 = 0, a3 = m;
    int64 b1 = 0, b2 = 1, b3 = b;
    
    while(1){
            
        if (b3 == 0){
            return 0; //no inverse
        }

        if(b3 == 1){
            int64 inv = b2 % m;
            if(inv < 0) inv += m;
            return (uint32) inv; //multiplicative inverse
        }

        int64 q = a3 / b3;

        int64 t1 = a1 - (q*b1);
        int64 t2 = a2 - (q*b2);
        int64 t3 = a3 - (q*b3);

        a1 = b1;
        a2 = b2;
        a3 = b3;

        b1 = t1;
        b2 = t2;
        b3 = t3;
    }
}

uint32 montgomery_reduce(uint64 x,uint32 n){
    uint64 t1 = x;
    uint64 t2 = (uint64)((uint32)x * extended_ecluid(R,n))*n;
    uint32 r = (uint32)((t1 - t2) >> 32);
    r += n & -(t1 < t2);
    return r;
}

uint32 montgomery_mul(uint32 a, uint32 b) {
    return mont_reduce((uint64)a * b);
}



int main(){

    uint32 result = extended_ecluid(26,7);
    printf("%u",result);

    return 0;
}