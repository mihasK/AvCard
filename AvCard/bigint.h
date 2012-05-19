#ifndef __BIGINT_H_
#define __BIGINT_H_
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "typedef.h"

#define MAX_LEN 16
#define MAX_VALUE ((1ULL << 32) - 1)

class BigInteger {
public:
	size_t length;
	uint32 data[1 + MAX_LEN];


	void LevelUp();
	bool getBit(const uint32 &at) const;

public:
	BigInteger();

	void reduce(uint32 t) {
		for (; t < length; ++t) this->data[t] = 0U;
	}
	BigInteger(byte *data, int length) ;
	BigInteger(uint32 t);

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
	friend BigInteger operator <<(const BigInteger &a, const uint32 &n) {
		BigInteger ret = a;
		ret <<= 1;
		return ret;
	}



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

#endif