
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "typedef.h"

#define MAX_LEN 64
#define MAX_VALUE ((1ULL << 32) - 1)

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
class BigInteger {
private:
	size_t length;
	uint32 data[1 + MAX_LEN];


	void LevelUp();

public:
	BigInteger();


	BigInteger(byte *data, int length) ;

	byte* getData() const;
	int getLength() const;

	BigInteger& operator=(const BigInteger &another);


	friend bool operator==(const BigInteger &a, const BigInteger &b) ;
	friend bool operator >(const BigInteger &a, const BigInteger &b) ;
	friend bool operator <(const BigInteger &a, const BigInteger &b) ;

	friend bool operator <=(const BigInteger &a, const BigInteger &b);

	friend bool operator >=(const BigInteger &a, const BigInteger &b) ;

	BigInteger& operator+=(const BigInteger& another) ;
	//works only if this >= another, must be checked on higher level
	BigInteger& operator-=(const BigInteger& another);

	BigInteger& operator*=(const uint32& b) ;
	BigInteger& operator *= (const BigInteger& another);

	BigInteger& operator<<=(const uint32 &n);
	BigInteger &operator>>=(const uint32 &n);



	BigInteger& operator/=(const BigInteger& another);

	BigInteger& operator %= (const BigInteger &another) ;

	friend BigInteger operator % (const BigInteger &a, const BigInteger &b) ;

	friend BigInteger operator/ (const BigInteger &a, const BigInteger &b);

	friend BigInteger operator+(const BigInteger &a, const BigInteger& b) ;

	friend BigInteger operator-(const BigInteger &a, const BigInteger& b) ;

	friend BigInteger operator *(const BigInteger &a, const BigInteger& b) ;
	friend BigInteger operator *(const BigInteger &a, const uint32 b) ;

	BigInteger pow(uint32 n) ;

	BigInteger powMod(uint32 n, const BigInteger &mod);


};