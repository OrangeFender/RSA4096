#define MAXqwords2048 2048/(sizeof(uint64_t)*8)
#define MAXqwords4096 4096/(sizeof(uint64_t)*8)

#include <iostream>
#include <iomanip>
#include <cstring>
#include "Karatsuba.h"

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

class uInt2048;
class uInt4096{
    uint64_t digits[MAXqwords4096];
    public:
    uInt4096 operator+(const uInt4096& other) const;
    uInt4096 operator-(const uInt4096& other) const;
    uInt4096();
    uInt4096(const uint64_t*Digit);
    uInt2048 leftshift2048();
    uInt4096 operator>>(int n) const;
    uInt4096 operator*(const uInt4096& other) const;
    uInt2048 operator%(const uInt2048& other) const;\
    uInt2048 module(const uInt2048& other) const;
};

uInt4096::uInt4096(){
    for(int i = 0; i < MAXqwords4096;i++){
        digits[i] =0;
    }
}
uInt4096::uInt4096(const uint64_t*Digit){
    memcpy((void*)digits,(void*)Digit,MAXqwords4096*sizeof(uint64_t));
}

uInt4096 uInt4096::operator+(const uInt4096& other) const{
    uInt4096 ret;
    add_qword(other.digits,digits,ret.digits,MAXqwords4096);
    return ret;
}

uInt4096 uInt4096::operator-(const uInt4096& other) const{
    uInt4096 ret;
    sub_qword(other.digits,digits,ret.digits,MAXqwords4096);
    return ret;
}

uInt4096 uInt4096::operator>>(int n) const{
    uInt4096 ret;
    int bigshift=n/64;
    int smallshift=n%64;
    for(int i=0;i<MAXqwords4096;i++){
        if(i+bigshift+1<MAXqwords4096){
            ret.digits[i]|=(digits[i+bigshift]>>smallshift);
        }
        if(i+bigshift<MAXqwords4096){
            ret.digits[i]|=(digits[i+bigshift+1]<<(64-smallshift));
        }
    }
}

class uInt2048{
    uint64_t digits[MAXqwords2048];
    public:
    uInt2048 operator+(const uInt2048& other) const;
    uInt2048 operator-(const uInt2048& other) const;
    uInt2048();
    uInt2048(const uint64_t*Digit);
    uInt2048(const std::string& Str);
    uInt2048 uInt2048byHEX(const std::string& Str);
    uInt2048 operator*(const uInt2048& other) const;
    void printHEX();
    bool operator==(const uInt2048& other) const;
    
};
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
    digits[index] =tmp;
}
uInt2048 uInt4096::leftshift2048(){
    uInt2048 ret(digits+MAXqwords2048);
    return ret;
}

uInt2048 uInt2048::operator+(const uInt2048& other) const{
    uInt2048 ret;
    add_qword(other.digits,digits,ret.digits,MAXqwords2048);
    return ret;
}

uInt2048 uInt2048::operator-(const uInt2048& other) const{
    uInt2048 ret;
    sub_qword(other.digits,digits,ret.digits,MAXqwords2048);
    return ret;
}




uInt2048 uInt2048::operator*(const uInt2048& other) const{
    uInt2048 ret;
    uint64_t tmp[MAXqwords2048*2];
    karatsuba((const uint32_t *)digits,(const uint32_t *)other.digits,(uint32_t *)tmp,MAXqwords2048*2);
    memcpy((void*)ret.digits,tmp,MAXqwords2048*sizeof(uint64_t));
    return ret;
}

uInt4096 uInt4096::operator*(const uInt4096& other)const{
    uInt4096 ret;
    uint64_t tmp[2*MAXqwords4096];
    karatsuba((const uint32_t *)digits,(const uint32_t *)other.digits,(uint32_t *)tmp,MAXqwords4096*2);
    memcpy((void*)ret.digits,tmp,MAXqwords4096*sizeof(uint64_t));
    return ret;
}

bool uInt2048::operator==(const uInt2048& other) const{
    for(int i=0;i<MAXqwords2048;i++){
        if(digits[i]!=other.digits[i])return false;
    }
    return true;
}

void uInt2048::printHEX(){
    int i=MAXqwords2048-1;

    while(i>=0){
        std::cout<<std::hex<<std::setw(16)<< std::setfill('0')<<digits[i];
        i--;
    }
    std::cout<<std::endl;
    return;
}

