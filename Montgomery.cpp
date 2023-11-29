#include"BigInt.h"

uInt2048 REDC(uInt4096 T,uInt2048 N,uInt2048 N_){
    uInt2048 TmodR=T.convert2048();
    uInt2048 m=(TmodR*N_).convert2048();
    uInt2048 t=(T+m*N).rightshift2048();
    if(t>=N){
        return t-N;
    }
    else{
        return t;
    }
}

uInt2048 XGCD(uInt2048 a,uInt2048 b,uInt2048& x,uInt2048& y){
    if(b==0){
        x=uInt2048("1");
        y=uInt2048("0");
        return a;
    }
    else{
        uInt2048 r=XGCD(b,a%b,x,y);
        uInt2048 t=x;
        x=y;
        y=t-((a/b)*y).convert2048();
        return r;
    }
}

