#define MAXqwords 2048/(sizeof(uint64_t)*8)
#define dwordmask 0x00000000ffffffff

#include <iostream>


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

class uInt2048{
    uint64_t digits[MAXqwords];
    public:
    uInt2048 operator+(const uInt2048& other) const;
    uInt2048 operator-(const uInt2048& other) const;
    uInt2048();
    uInt2048(const std::string& Str);
    uInt2048 uInt2048byHEX(const std::string& Str);
    uInt2048 operator*(const uInt2048& other) const;
    void printHEX();
};
uInt2048::uInt2048(){
    for(int i = 0; i < MAXqwords;i++){
        digits[i] =0;
    }
}
uInt2048::uInt2048(const std::string& Str){
    for(int i = 0; i < MAXqwords;i++){
        digits[i] =0;
    }
    int len = Str.length();
    uint64_t tmp=0;
    int j=0;
    int index=0;
    for(int i = len-1; i >=0;i--){
        if(j==15){//算完一个数就将其放入数组中
            digits[index] =tmp;
            j=0;
            index++;
            tmp=0;
        }
        tmp|=hexCharToInt(Str[i])<<(j*4);
        j++;
    }
    digits[index] =tmp;
}

int add_qword(const uint64_t* a,const uint64_t* b,uint64_t* res, int len){
    int carry=0;   
    for(int i=0; i<len;i++){
        carry=__builtin_uaddll_overflow(a[i],carry,&res[i]);
        carry|=__builtin_uaddll_overflow(res[i],b[i],&res[i]);
    }
    return carry;
}
int sub_qword(const uint64_t* a,const uint64_t* b,uint64_t* res, int len){
    int carry=0;   
    for(int i=0; i<len;i++){
        carry=__builtin_usubll_overflow(a[i],carry,&res[i]);
        carry|=__builtin_usubll_overflow(res[i],b[i],&res[i]);
    }
    return carry;
}

uInt2048 uInt2048::operator+(const uInt2048& other) const{
    uInt2048 ret;
    add_qword(other.digits,digits,ret.digits,MAXqwords);
    return ret;
}

uInt2048 uInt2048::operator-(const uInt2048& other) const{
    uInt2048 ret;
    sub_qword(other.digits,digits,ret.digits,MAXqwords);
    return ret;
}


//乘法最小输入为dword结果正好为一个qword，这样可以保证没有溢出
//该函数的作用是将a与b相乘（长度不定）并将结果加或减到结果数中
//a,b长度相等
void karatsuba(const uint32_t *a,const uint32_t *b,uint32_t* result,int len){
if(len==1){
    ((uint64_t*)result)[0]=((uint64_t)a[0]&dwordmask)*((uint64_t)b[0]&dwordmask);
    return;
}
if(len==2){
        int halflen =len/2;
    int resultlen=2*len;
    uint32_t* A_B=new uint32_t[halflen];
    uint32_t* C_D=new uint32_t[halflen];
    int A_Bcarry=__builtin_uadd_overflow(a[0],a[halflen],A_B);
    int C_Dcarry=__builtin_uadd_overflow(b[0],b[halflen],C_D);
    uint32_t* AD_BC=new uint32_t[len];
    karatsuba(A_B,C_D,AD_BC,halflen);
    int anotherCarry=0;
    if(A_Bcarry)anotherCarry+=__builtin_uadd_overflow(C_D[0],(AD_BC[halflen]),(AD_BC+halflen));
    if(C_Dcarry)anotherCarry+=__builtin_uadd_overflow(A_B[0],(AD_BC[halflen]),(AD_BC+halflen));
    int allcarry=anotherCarry+A_Bcarry*C_Dcarry;
    karatsuba(a,b,result,halflen);
    karatsuba(a+halflen,b+halflen,result+len,halflen);//这里的两个乘法互不干扰，所以直接赋值给result
    allcarry-=sub_qword((uint64_t*)AD_BC,(uint64_t*)(result+len),(uint64_t*)AD_BC,halflen);
    allcarry-=sub_qword((uint64_t*)AD_BC,(uint64_t*)(result),(uint64_t*)AD_BC,halflen);
    allcarry+=add_qword((uint64_t*)AD_BC,(uint64_t*)(result+halflen),(uint64_t*)(result+halflen),halflen);
    int i=0;
    while (allcarry&&i<halflen)
    {
        allcarry=__builtin_uadd_overflow(allcarry,result[halflen+len+i],result+halflen+len+i);
        i++;
    }
    delete[] A_B;
    delete[] C_D;
    delete[] AD_BC;
    }

else{

    int halflen =len/2;
    int resultlen=2*len;
    uint32_t* A_B=new uint32_t[halflen];
    uint32_t* C_D=new uint32_t[halflen];
    int A_Bcarry=add_qword((uint64_t*)a,(uint64_t*)(a+halflen),(uint64_t*)A_B,halflen/2);
    int C_Dcarry=add_qword((uint64_t*)b,(uint64_t*)(b+halflen),(uint64_t*)C_D,halflen/2);
    uint32_t* AD_BC=new uint32_t[len];
    karatsuba(A_B,C_D,AD_BC,halflen);
    if(A_Bcarry)A_Bcarry+=add_qword((uint64_t*)C_D,(uint64_t*)(AD_BC+halflen),(uint64_t*)(AD_BC+halflen),halflen/2);
    if(C_Dcarry)C_Dcarry+=add_qword((uint64_t*)A_B,(uint64_t*)(AD_BC+halflen),(uint64_t*)(AD_BC+halflen),halflen/2);
    int allcarry=A_Bcarry+C_Dcarry;
    karatsuba(a,b,result,halflen);
    karatsuba(a+halflen,b+halflen,result+len,halflen);
    allcarry-=sub_qword((uint64_t*)AD_BC,(uint64_t*)(result+len),(uint64_t*)AD_BC,halflen);
    allcarry-=sub_qword((uint64_t*)AD_BC,(uint64_t*)(result),(uint64_t*)AD_BC,halflen);
    allcarry+=add_qword((uint64_t*)AD_BC,(uint64_t*)(result+halflen),(uint64_t*)(result+halflen),halflen);
    int i=0;
    while (allcarry&&i<halflen)
    {
        allcarry=__builtin_uadd_overflow(allcarry,result[halflen+len+i],result+halflen+len+i);
        i++;
    }
    delete[] A_B;
    delete[] C_D;
    delete[] AD_BC;
    }
}

uInt2048 uInt2048::operator*(const uInt2048& other) const{
    uInt2048 ret;
    uint64_t tmp[2*MAXqwords];
    karatsuba((const uint32_t *)digits,(const uint32_t *)other.digits,(uint32_t *)tmp,MAXqwords);
    memcpy((void*)ret.digits,tmp,MAXqwords*sizeof(uint64_t));
    return ret;
}

void uInt2048::printHEX(){
    int i=MAXqwords-1;

    while(i>=0){
        std::cout<<std::hex<<digits[i];
        i--;
    }
    std::cout<<std::endl;
    return;
}

