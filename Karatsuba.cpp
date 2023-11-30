#include <stdint.h>
#define dwordmask 0x00000000ffffffff

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
    int anotherCarry=0;
    if(A_Bcarry)anotherCarry+=add_qword((uint64_t*)C_D,(uint64_t*)(AD_BC+halflen),(uint64_t*)(AD_BC+halflen),halflen/2);
    if(C_Dcarry)anotherCarry+=add_qword((uint64_t*)A_B,(uint64_t*)(AD_BC+halflen),(uint64_t*)(AD_BC+halflen),halflen/2);
    int allcarry=anotherCarry+A_Bcarry*C_Dcarry;
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