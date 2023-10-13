#include<iostream>
#include"Multiplication.h"

int main(){
    uInt2048 a("123456789abcdef");
    uInt2048 b("123456789abcdef");
    a.printHEX();
    b.printHEX();
    uInt2048 c=a*b;
    c.printHEX();
    return 0;

}