#include <stdio.h>

typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

#define R 0x100000000ULL

void read_hex(FILE *f,uint32*x){
    fscanf(f, "%x", x);
}

void write_hex(FILE *f, uint32 x){
    fprintf(f, "%08X\n", x);
}


uint32 extended_ecluid(uint64 m ,uint64 b){
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

uint32 montgomery_mul(uint32 a, uint32 b, uint32 n) {
    return montgomery_reduce((uint64)a * b, n);
}

uint64 calc_r2(uint32 n){
    return (uint64)(R%n)*(R%n)%n;
}

uint32 montgomery_exp(uint32 base, uint32 exp, uint32 n) {
    uint64 R2 = calc_r2(n);
    uint32 x = montgomery_reduce((uint64)base * R2,n);  
    uint32 acc = montgomery_reduce(R2,n);                

    while (exp) {
        if (exp & 1) acc = montgomery_mul(acc, x, n);
        x = montgomery_mul(x, x, n);
        exp >>= 1;
    }
    return montgomery_reduce(acc,n); 
}

int main(int argc, char **argv){
    if (argc < 4) return 1;

    char mode = argv[1][0];
    FILE *kf = fopen(argv[2], "r");
    if (!kf) return 1;

    uint32 key_exp = 0,n=0;
    read_hex(kf, &key_exp);
    read_hex(kf,&n);
    fclose(kf);

    if(mode == 'g'){
        FILE *pk = fopen(argv[2], "r");
        FILE *sk = fopen(argv[3], "w");
        if(!pk || !sk) return 1;

        uint32 e = 0;
        read_hex(pk,&e);
        fclose(pk);

        uint32 phi = 0x0001E508;
        uint32 d = extended_ecluid(phi, e);

        fprintf(sk, "%08X %08X\n", d, n);
        fclose(sk);
        return 0;
    }

    FILE *in = fopen(argv[3], "r");
    FILE *out = fopen(argv[4], "w");
    if(!in || !out) return 1;

    uint32 data = 0;
    read_hex(in, &data);

    uint32 result = montgomery_exp(data, key_exp, n);
    write_hex(out, result);
    fclose(out);

    return 0;
}