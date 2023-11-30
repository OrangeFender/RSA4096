#pragma once
#include <iostream>
#define MAXqwords2048 2048/(sizeof(uint64_t)*8)
#define MAXqwords4096 4096/(sizeof(uint64_t)*8)

class uInt2048;
class uInt4096{
    uint64_t digits[MAXqwords4096];
    friend class uInt2048;
    uInt4096(const uint64_t*Digit);
    public:
    uInt4096 operator+(const uInt4096& other) const;
    uInt4096 operator-(const uInt4096& other) const;
    uInt4096();
    
    uInt4096(const std::string& Str);
    uInt2048 rightshift2048();
    uInt4096 operator>>(int n) const;
    uInt4096 operator*(const uInt4096& other) const;
    uInt2048 operator%(const uInt2048& other) const;
    uInt4096 operator/(const uInt4096& other) const;
    bool operator==(const uInt4096& other) const;
    bool operator>=(const uInt4096& other) const;
    
    uInt2048 convert2048();
    void printHEX()const;
};

class uInt2048{
    uint64_t digits[MAXqwords2048];
    friend class uInt4096;
    public:
    uInt2048 operator+(const uInt2048& other) const;
    uInt2048 operator-(const uInt2048& other) const;
    uInt2048();
    uInt2048(const uint64_t*Digit);
    uInt2048(const std::string& Str);
    uInt4096 operator*(const uInt2048& other) const;
    void printHEX()const;
    bool operator==(const uInt2048& other) const;
    uInt2048 operator%(const uInt2048& other) const;
    uInt2048 operator>>(int n) const;
    bool operator>= (const uInt2048& other) const;
    uInt4096 leftshift2048();
    uInt4096 convert4096()const;
    uInt2048 operator/(const uInt2048& other) const;
};

