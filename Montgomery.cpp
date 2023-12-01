#include"BigInt.h"
#include"Montgomery.h"

uInt2048 MontgomeryInt::N;
uInt2048 MontgomeryInt::N_;

uInt2048 REDC(uInt4096 T,uInt2048 N,uInt2048 N_){
    uInt2048 TmodR=T.convert2048();
    uInt2048 m=(TmodR*N_).convert2048();

    //(T+m*N).printHEX();

    uInt2048 t=(T+m*N).rightshift2048();

    //t.printHEX();

    if(t>=N){
        return t-N;
    }
    else{
        return t;
    }
}

uInt2048 XGCD(uInt2048 *a,uInt2048 *b,uInt2048& x,uInt2048& y){
    const static uInt2048 zero;
    if(*b==zero){
        x=uInt2048("1");
        y=uInt2048("0");
        return *a;
    }
    else{
    uInt2048 *remainder =new uInt2048;
    *remainder=*a%*b;
    uInt2048 gcd = XGCD(b, remainder, x, y);
    uInt2048* t =new uInt2048;
    *t=x;
    x = y;
    y = *t - ((*a/(*b)) * y).convert2048();

    delete remainder;
    delete t;
    
    return gcd;
    }
    
}

void MontgomeryInt::setN(uInt2048 n){

    N=n;
    uInt2048 zero;
    uInt4096 R=uInt2048("1").leftshift2048();//由于XGCD是2048位模2048位，R是2049位，所以不支持直接计算R
    uInt2048 x,y;
    uInt2048 q=(R/N.convert4096()).convert2048();
    uInt2048 RmodN=(R-q*N).convert2048();
    XGCD(&N,&RmodN,x,y);
    //x.printHEX();
    //y.printHEX();
    uInt2048 t=x;
    x=y;
    y=t-(q*y).convert2048();
    N_=zero-y;

    //N.printHEX();
    //N_.printHEX();
}

void MontgomeryInt::setNandN_(uInt2048 n,uInt2048 n_){
    N=n;
    N_=n_;
}

MontgomeryInt::MontgomeryInt(){
    state=uInt2048("0");
}

MontgomeryInt::MontgomeryInt(uInt2048 a){
    state=a.leftshift2048()%N;
    //state.printHEX();
}

MontgomeryInt MontgomeryInt::operator*(const MontgomeryInt& other) const{
    MontgomeryInt ret;
    uInt4096 T=state.convert4096()*other.state.convert4096();
    //T.printHEX();
    ret.state=REDC(T,N,N_);
    return ret;
}

uInt2048 MontgomeryInt::convert()const{
    uInt2048 ret=REDC(state.convert4096(),N,N_);
    return ret;
}