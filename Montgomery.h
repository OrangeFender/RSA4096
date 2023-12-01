#include"BigInt.h"
#include<iostream>

//存储已经转换为蒙哥马利形式的整数
class MontgomeryInt{
    uInt2048 state;
    public:
    static uInt2048 N;//模数使用静态变量
    static uInt2048 N_;
    MontgomeryInt(uInt2048 a);
    MontgomeryInt();
    MontgomeryInt operator*(const MontgomeryInt& other) const;
    MontgomeryInt operator+(const MontgomeryInt& other) const;
    uInt2048 convert()const;
    static void setN(uInt2048 N);
    static void setNandN_(uInt2048 n,uInt2048 n_);
};