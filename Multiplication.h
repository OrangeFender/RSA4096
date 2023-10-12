#pragma once
#include <iostream>
#define MAXqwords 2048/sizeof(uint64_t)

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