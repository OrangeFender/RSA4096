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



//乘法最小输入为两个dword结果正好为一个qword，这样可以保证没有溢出
//该函数的作用是将a与b相乘（长度不定）并将结果加或减到结果数中
//a,b长度相等
//len为a,b的长度，单位为dword

void karatsuba(const uint32_t *a,const uint32_t *b,uint32_t* result,int len){
    /*
        Karatsuba算法图示:
            A  B
         ×  C  D
        -----------------
            AD BD
        +AC BC

        AD+BC = (A+B)(C+D)-AC-BD


        AC, BD 不会产生溢出，但是AD+BC可能会产生溢出，所以需要特殊处理。
        而实际处理更为复杂，因为计算AD+BC需要先计算A+B，C+D，此时就会发生溢出。
        先用A_Bcarry和C_Dcarry记录A+B和C+D是否发生了溢出，如果发生了溢出，那么计算(A+B)(C+D)时要加上溢出的部分，注意溢出最多为1，因此只需要加一次A+B或C+D即可。
        allcarry用于最终记录AD+BC的溢出情况，注意这里的溢出部分可能会导致传递，因此需要循环处理。
    */
if(len==1){//长度为一直接相乘即可
    ((uint64_t*)result)[0]=((uint64_t)a[0]&dwordmask)*((uint64_t)b[0]&dwordmask);
    return;
}
else
if(len==2){//长度为2时，由于加法的最小单位为dword，因此需要单独拿出来处理
    
        int halflen =len/2;
    int resultlen=2*len;
    uint32_t A_B[len/2];//A+B
    uint32_t C_D[len/2];//C+D
    int A_Bcarry=__builtin_uadd_overflow(a[0],a[halflen],A_B);
    int C_Dcarry=__builtin_uadd_overflow(b[0],b[halflen],C_D);
    uint32_t AD_BC[len];//用于存放 AD+BC
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
    return;
    }

else{

    int halflen =len/2;
    int resultlen=2*len;
    uint32_t A_B[halflen];//A+B
    uint32_t C_D[halflen];//C+D
    int A_Bcarry=add_qword((uint64_t*)a,(uint64_t*)(a+halflen),(uint64_t*)A_B,halflen/2);
    int C_Dcarry=add_qword((uint64_t*)b,(uint64_t*)(b+halflen),(uint64_t*)C_D,halflen/2);
    uint32_t AD_BC[len];//用于存放 AD+BC
    karatsuba(A_B,C_D,AD_BC,halflen);
    int anotherCarry=0;
    if(A_Bcarry)anotherCarry+=add_qword((uint64_t*)C_D,(uint64_t*)(AD_BC+halflen),(uint64_t*)(AD_BC+halflen),halflen/2);
    if(C_Dcarry)anotherCarry+=add_qword((uint64_t*)A_B,(uint64_t*)(AD_BC+halflen),(uint64_t*)(AD_BC+halflen),halflen/2);
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

    return;
    }
}