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

