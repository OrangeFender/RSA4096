#define MAXqwords2048 2048/(sizeof(uint64_t)*8)
#define MAXqwords4096 4096/(sizeof(uint64_t)*8)

#include <iostream>
#include <iomanip>
#include <cstring>
#include "Karatsuba.h"

#include "BigInt.h"

static uInt2048 zero2048=uInt2048("0");

uint64_t hexCharToInt(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return 10 + c - 'a';
    } else if (c >= 'A' && c <= 'F') {
        return 10 + c - 'A';
    }
    return -1; // 非法字符
}

int static add_qword(const uint64_t* a,const uint64_t* b,uint64_t* res, int len){
    int carry=0;   
    for(int i=0; i<len;i++){
        uint64_t tmp;
        carry=__builtin_uaddll_overflow(a[i],carry,&tmp);
        carry|=__builtin_uaddll_overflow(tmp,b[i],&res[i]);
    }
    return carry;
}
int static sub_qword(const uint64_t* a,const uint64_t* b,uint64_t* res, int len){
    int carry=0;   
    for(int i=0; i<len;i++){
        uint64_t tmp;
        carry=__builtin_usubll_overflow(a[i],carry,&tmp);
        carry|=__builtin_usubll_overflow(tmp,b[i],&res[i]);
    }
    return carry;
}

uInt4096::uInt4096(){
    for(int i = 0; i < MAXqwords4096;i++){
        digits[i] =0;
    }
}

uInt4096::uInt4096(const uint64_t*Digit){
    memcpy((void*)digits,(void*)Digit,MAXqwords4096*sizeof(uint64_t));
}

uInt4096::uInt4096(const std::string& Str){
    for(int i = 0; i < MAXqwords4096;i++){
        digits[i] =0;
    }
    int len = Str.length();
    if(len>4096/4){
        std::cout<<"error"<<std::endl;
        return;
    }
    uint64_t tmp=0;
    int j=0;
    int index=0;
    for(int i = len-1; i >=0;i--){
        tmp|=hexCharToInt(Str[i])<<(j*4);
        j++;
        if(j==16){//算完一个数就将其放入数组中
            digits[index] =tmp;
            j=0;
            index++;
            tmp=0;
        }
    }
    if(index<MAXqwords4096)
        digits[index] =tmp;
}

uInt4096 uInt4096::operator+(const uInt4096& other) const{
    uInt4096 ret;
    add_qword(other.digits,digits,ret.digits,MAXqwords4096);
    return ret;
}

uInt4096 uInt4096::operator-(const uInt4096& other) const{
    uInt4096 ret;
    sub_qword(digits,other.digits,ret.digits,MAXqwords4096);
    return ret;
}

uInt4096 uInt4096::operator>>(int n) const{//4096位右移n位
    uInt4096 ret;
    int bigshift=n/64;
    int smallshift=n%64;
    ret.digits[MAXqwords4096-1]=digits[MAXqwords4096-1+bigshift]>>smallshift;
    for(int i=MAXqwords4096-2-bigshift;i>=0;i--){
        ret.digits[i]|=digits[i+bigshift+1]<<(64-smallshift);
        ret.digits[i]|=digits[i+bigshift]>>smallshift;
    }
    return ret;
}

bool uInt4096::operator==(const uInt4096& other) const{//判断相等
    for(int i=0;i<MAXqwords4096;i++){
        if(digits[i]!=other.digits[i])return false;
    }
    return true;
}

bool uInt4096::operator>=(const uInt4096& other) const{
    for(int i=MAXqwords4096-1;i>=0;i--){
        if(digits[i]>other.digits[i])return true;
        if(digits[i]<other.digits[i])return false;
    }
    return true;
}

uInt2048::uInt2048(const uint64_t*Digit){
    memcpy((void*)digits,(void*)Digit,MAXqwords2048*sizeof(uint64_t));
}

uInt2048::uInt2048(){
    for(int i = 0; i < MAXqwords2048;i++){
        digits[i] =0;
    }
}

uInt2048::uInt2048(const std::string& Str){
    for(int i = 0; i < MAXqwords2048;i++){
        digits[i] =0;
    }
    int len = Str.length();
    if(len>2048/4){
        std::cout<<"error"<<std::endl;
        return;
    }
    uint64_t tmp=0;
    int j=0;
    int index=0;
    for(int i = len-1; i >=0;i--){
        tmp|=hexCharToInt(Str[i])<<(j*4);
        j++;
        if(j==16){//算完一个数就将其放入数组中
            digits[index] =tmp;
            j=0;
            index++;
            tmp=0;
        }
    }
    if(index<MAXqwords2048)
        digits[index] =tmp;
}

uInt2048 uInt4096::convert2048(){//将4096位转化为2048位
    uInt2048 ret(digits);
    return ret;
}

uInt4096 uInt2048::convert4096()const{//将2048位转化为4096位
    uInt4096 ret;
    memcpy((void*)ret.digits,(void*)digits,MAXqwords2048*sizeof(uint64_t));
    return ret;
}

uInt2048 uInt4096::rightshift2048(){//4096位右移2048位
    uInt2048 ret(digits+MAXqwords2048);
    return ret;
}

uInt2048 uInt2048::operator+(const uInt2048& other) const{//两2048bit的加法
    uInt2048 ret;
    add_qword(other.digits,digits,ret.digits,MAXqwords2048);
    return ret;
}

uInt2048 uInt2048::operator-(const uInt2048& other) const{//减法
    uInt2048 ret;
    sub_qword(digits,other.digits,ret.digits,MAXqwords2048);
    return ret;
}

uInt4096 uInt2048::operator*(const uInt2048& other) const{//两2048bit的karatsuba乘法
    uInt4096 ret;
    uint64_t tmp[MAXqwords2048*2];
    karatsuba((const uint32_t *)digits,(const uint32_t *)other.digits,(uint32_t *)tmp,MAXqwords2048*2);
    memcpy((void*)ret.digits,tmp,MAXqwords4096*sizeof(uint64_t));
    return ret;
}

uInt4096 uInt4096::operator*(const uInt4096& other)const{//两4096bit的karatsuba乘法
    uInt4096 ret;
    uint64_t tmp[2*MAXqwords4096];
    karatsuba((const uint32_t *)digits,(const uint32_t *)other.digits,(uint32_t *)tmp,MAXqwords4096*2);
    memcpy((void*)ret.digits,tmp,MAXqwords4096*sizeof(uint64_t));
    return ret;
}

bool uInt2048::operator==(const uInt2048& other) const{//判断相等
    for(int i=0;i<MAXqwords2048;i++){
        if(digits[i]!=other.digits[i])return false;
    }
    return true;
}

bool uInt2048::operator>=(const uInt2048& other) const{//判断大于等于
    for(int i=MAXqwords2048-1;i>=0;i--){
        if(digits[i]>other.digits[i])return true;
        if(digits[i]<other.digits[i])return false;
    }
    return true;
}

uInt2048 uInt2048::operator>>(int n) const{//2048位右移n位
    uInt2048 ret;
    int bigshift=n/64;
    int smallshift=n%64;
    ret.digits[MAXqwords2048-1-bigshift]=digits[MAXqwords2048-1]>>smallshift;
    for(int i=MAXqwords2048-2-bigshift;i>=0;i--){
        ret.digits[i]|=digits[i+bigshift+1]<<(64-smallshift);
        ret.digits[i]|=digits[i+bigshift]>>smallshift;
    }
    return ret;
}

void uInt2048::printHEX()const{
    int i=MAXqwords2048-1;

    while(i>=0){
        std::cout<<std::hex<<std::setw(16)<< std::setfill('0')<<digits[i];
        i--;
    }
    std::cout<<std::endl;
    return;
}


void uInt4096::printHEX()const{
    int i=MAXqwords4096-1;

    while(i>=0){
        std::cout<<std::hex<<std::setw(16)<< std::setfill('0')<<digits[i];
        i--;
    }
    std::cout<<std::endl;
    return;
}



uInt4096 uInt2048::leftshift2048(){//2048位左移2048位
    uInt4096 ret;
    memcpy((void*)(ret.digits+MAXqwords2048),(void*)digits,MAXqwords2048*sizeof(uint64_t));
    return ret;
}

int find_highestbit_2048(uint64_t * digits){
    int k=0;
    for(int i=MAXqwords2048-1;i>=0;i--){
        for(int j=63;j>=0;j--){
            if(((digits[i]>>j)&1)){
                k=i*64+j;
                return k;
            }
        }
    }
    return k;
}

int find_highestbit_4096(uint64_t * digits){
    int k=0;
    for(int i=MAXqwords4096-1;i>=0;i--){
        for(int j=63;j>=0;j--){
            if(((digits[i]>>j)&1)){
                k=i*64+j;
                return k;
            }
        }
    }
    return k;
}

uInt2048 uInt2048::operator/(const uInt2048& other) const{//2048位除2048位
    uInt2048 ret;
    int k=find_highestbit_2048((uint64_t*)other.digits);
    int l=find_highestbit_2048((uint64_t*)digits);
    if(l<k){
        return ret;
    }
    else{
        int n=l-k;
        int sbits=n%64;
        int qwords=n/64;
        while(qwords>=0){
            while (sbits>=0){
                ret.digits[qwords]^=((uint64_t)1<<sbits);//先将这一位设为1
                uInt2048 tmp=(ret*other).convert2048();
                if(tmp>=*this){
                    ret.digits[qwords]^=((uint64_t)1<<sbits);//如果大于就将这一位设为0
                }
                sbits--;
            }
            qwords--;
            sbits=63;
        }
    }
    return ret;
}

uInt4096 uInt4096::operator/(const uInt4096& other)const{//4096位除4096位
    uInt4096 ret;
    int k=find_highestbit_4096((uint64_t*)other.digits);
    int l=find_highestbit_4096((uint64_t*)digits);
    if(l<k){
        return ret;
    }
    else{
        int n=l-k;
        int sbits=n%64;
        int qwords=n/64;
        while(qwords>=0){
            while (sbits>=0){
                ret.digits[qwords]^=((uint64_t)1<<sbits);//先将这一位设为1
                uInt4096 tmp=(ret*other);
                if(tmp>=*this){
                    ret.digits[qwords]^=((uint64_t)1<<sbits);//如果大于就将这一位设为0
                }
                sbits--;
            }
            qwords--;
            sbits=63;
        }
    }
    return ret;
}



uInt2048 uInt2048::operator%(const uInt2048& other) const{//2048位模2048位
//固定k为2048
uInt2048 m;
if(other==zero2048){
    return zero2048;
}

//利用二进制除法计算m，m=2^2048/q
int k=find_highestbit_2048((uint64_t*)other.digits);

int l=2049;

int n=l-k;
int sbits=n%64;
int qwords=n/64;
while(qwords>=0){
    while (sbits>=0){
        m.digits[qwords]^=((uint64_t)1<<sbits);//先将这一位设为1
        uInt4096 tmp=(m*other);
        if(tmp.digits[MAXqwords2048]>=1){
            m.digits[qwords]^=((uint64_t)1<<sbits);//如果大于就将这一位设为0
        }
        sbits--;
    }
    qwords--;
    sbits=63;
}

//m.printHEX();

uInt2048 t=(m*(*this)).rightshift2048();
uInt2048 s=(t*other).convert2048();

uInt2048 tmp=*this-s;

if(tmp>=other){
    tmp=tmp-other;
    }
return tmp;

}

uInt2048 uInt4096::operator%(const uInt2048& other) const{//4096位模2048位
    uInt4096 other4096=other.convert4096();
    uInt4096 quotient=*this/(other4096);
    uInt4096 ret=*this-quotient*other4096;

    return ret.convert2048();
}