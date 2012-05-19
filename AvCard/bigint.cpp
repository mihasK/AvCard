#include "BigInt.h"
#include "helpers.h"

bool BigInteger::getBit(const uint32 &at) const {
		 return ::getBit(this->data, at);
}

void BigInteger::LevelUp()
{
	for (int i = length ;i>=1;i--)
		data[i] = data[i-1];
	if (data[length]) ++length;
}

BigInteger::BigInteger(uint32 t) {
	this->length = 1;
	this->data[0] = t;
}

BigInteger::BigInteger() {
	int once = 0;
	if (!once) {
		once = 1;
	}
	length = 1;
	data[0] = 0;
}

int BigInteger ::getLength() const{
	return this->length;
}


BigInteger::BigInteger(byte *data, int length) {
	this->length = 0;
	memset(this->data, 0, sizeof this->data);
	for (int at = length - 1; at >= 0; at -= 4) {
		for (int j = max(0, at - 3); j <= at; ++j) {
			this->data[this->length] <<= (sizeof byte) << 3;
			this->data[this->length] |= data[j];
		}
		++this->length;
	}		
	while (this->length > 1 && !this->data[this->length - 1]) --this->length;
}

BigInteger& BigInteger::operator=(const BigInteger &another) {
	if (this != &another) {
		memset(this->data, 0, sizeof this->data);
		for (size_t at = 0; at < another.length; this->data[at] = another.data[at++]);
		this->length = another.length;
	}
	return *this;
}


bool operator==(const BigInteger &a, const BigInteger &b) {
	if (a.length != b.length) return false;
	for (int i = a.length - 1; i >= 0; --i) if (a.data[i] != b.data[i]) return false;
	return true;
}
bool operator >(const BigInteger &a, const BigInteger &b) {
	if (a.length!=b.length) return a.length>b.length;
	for (int i=a.length-1;i>=0;i--)
	{
		if (a.data[i]!=b.data[i])
			return a.data[i]>b.data[i];
	}
	return false;
}
bool operator <(const BigInteger &a, const BigInteger &b) {
	if (a.length!=b.length) return a.length<b.length;
	for (int i=a.length-1;i>=0;i--)
	{
		if (a.data[i]!=b.data[i])
			return a.data[i]<b.data[i];
	}
	return false;
}

bool operator <=(const BigInteger &a, const BigInteger &b) {
	if (a.length!=b.length) return a.length<b.length;
	for (int i=a.length-1;i>=0;i--)
	{
		if (a.data[i]!=b.data[i])
			return a.data[i]< b.data[i];
	}
	return true;
}

bool operator >=(const BigInteger &a, const BigInteger &b) {
	if (a.length!=b.length) return a.length<b.length;
	for (int i=a.length-1;i>=0;i--)
	{
		if (a.data[i]!=b.data[i])
			return a.data[i]> b.data[i];
	}
	return true;
}

BigInteger& BigInteger:: operator+=(const BigInteger& another) {
	int carry = 0;
	for (size_t at = 0; at < another.length || carry; ++at) {
		if (at == this->length) ++this->length;
		unsigned long long actual = this->data[at]; actual += carry;
		if (at < another.length)	actual += another.data[at];
		if (actual > MAX_VALUE) {
			carry = 1;
		}
		this->data[at] = actual;
	}
	return *this;
}
//works only if this >= another, must be checked on higher level
BigInteger& BigInteger::operator-=(const BigInteger& another) {
	unsigned long long carry = 0;
	for (size_t at = 0; at < another.length || carry; ++at) {
		unsigned long long actualA = this->data[at], actualB = carry;
		if (at < another.length) actualB += another.data[at];
		if (actualA < actualB) {
			actualA += MAX_VALUE;
			actualA -= actualB;
			this->data[at] = actualA;
			carry = 1;				
		} else {
			this->data[at] -= actualB;
			carry = 0;
		}
	}
	while (this->length > 1 && this->data[this->length - 1] == 0) --this->length;
	return *this;
}

BigInteger& BigInteger::operator*=(const uint32& b) {
	unsigned long long carry = 0, actual;
	for (size_t at = 0; at < this->length || carry; ++at) {
		actual = 1ULL * this->data[at];
		actual *= b;
		actual += carry;
		this->data[at] = actual;
		carry = sizeof ( uint32 ) * 8;
	}
	while (this->length > 1 && this->data[this->length - 1] == 0) --this->length;
	return *this;
}
BigInteger& BigInteger::operator *= (const BigInteger& another) {
	uint32 *temp = new uint32[this->length];
	memcpy(temp, this->data, (sizeof uint32) * this->length);
	size_t len = this->length;
	size_t i , j ;
	uint32 carry ;
	uint64 mul ;
	for (size_t at = 0; at < (sizeof this->data) / sizeof this->data[0]; this->data[at++] = 0);
	for ( i = 0; i < another.length; ++ i ) {
		for ( j = 0, carry = 0; j < len; ++ j )
			((( mul = another.data [ i ]) *= temp [ j ]) += carry) += this->data [ i + j ] ,
			this->data [ i + j ] = mul ,
			carry = mul >> sizeof ( uint32 ) * 8;
		this->data [ i + j ] = carry ;
	}
	delete temp;
	return *this;
}

BigInteger& BigInteger::operator>>=(const uint32 &n) {
	if (n == 0) return *this;
	int nInts = n >> 5;
	int nBits = n & 0x1F;
	uint32 *mag = new uint32 [this->length];
	memcpy(mag, this->data, this->length * sizeof this->data[0]);
	int magLen = this->length;
	memset(this->data, 0, sizeof this->data);
	if (nInts >= magLen) {
		this->length = 1;
		return *this;
	}
	if (nBits == 0) {
		int newMagLen = magLen - nInts;
		for (int i=0; i<newMagLen; i++)
			this->data[i] = mag[i];
		this->length = newMagLen;
	} else {
		int newMagLength = 0;
		int i = 0;
		int highBits = mag[0] >> nBits;
		if (highBits != 0) {
			newMagLength =  magLen - nInts;
			this->data[i++] = highBits;
		} else {
			newMagLength = magLen - nInts -1;
		}

		int nBits2 = 32 - nBits;
		int j=0;
		while (j < magLen - nInts - 1)
			this->data[i++] = (mag[j++] << nBits2) | (mag[j] >> nBits);
		this->length = newMagLength;
	}
	return *this;
}

BigInteger& BigInteger::operator<<=(const uint32& n) {
	if (n == 0) return *this;
	int nInts = n >> 5;
	int nBits = n & 0x1F;
	uint32 *mag = new uint32 [this->length];
	memcpy(mag, this->data, this->length * sizeof this->data[0]);
	int magLen = this->length;
	memset(this->data, 0, sizeof this->data);
	if (nBits == 0) {
		this->length = magLen + nInts;
		for (int i=0; i<magLen; i++)
			this->data[i] = mag[i];
	} else {
		int i = 0;
		int newMagLength;
		int nBits2 = 32 - nBits;
		int highBits = mag[0] >> nBits2;
		if (highBits != 0) {
			newMagLength = magLen + nInts + 1;
			this->data[i++] = highBits;
		} else {
			newMagLength = magLen + nInts;
		}
		int j=0;
		while (j < magLen-1)
			this->data[i++] = mag[j++] << nBits | mag[j] >> nBits2;
		this->data[i] = mag[j] << nBits;
		this->length  = newMagLength;
	}
	return *this;
}


BigInteger& BigInteger::operator/=(const BigInteger& another) {
	BigInteger cur1, cur2;
	uint32 *digits = new uint32[this->length];
	memcpy(digits, this->data, this->length * (sizeof this->data[0]));
	memset(this->data, 0, sizeof this->data);
	int len = this->length;
	for (int i = len; i>=0; --i) {
		cur1.LevelUp();
		cur1.data[0] = digits[i];
		uint32 x = 0, l = 0, r = MAX_VALUE;
		while (l <= r) {
			uint32 m = (l + (r - l)) >> 1;
			cur2 = another * m;
			if (cur2 <= cur1) {
				x = m;
				l = m + 1;
			} else {
				r = m- 1;
			}
		}
		this->data[i] = x;
		cur1 = cur1 - another * x;
	}
	while (this->length > 1 && !this->data[this->length - 1]) --this->length;
	return *this;
}

BigInteger& BigInteger::operator%= (const BigInteger &another) {
	BigInteger act = (*this) / another;
	act *= another;
	(*this) -= act;
	return *this;
}

BigInteger operator % (const BigInteger &a, const BigInteger &b) {
	BigInteger ret = a;
	ret %= b;
	return ret;
}

BigInteger operator/ (const BigInteger &a, const BigInteger &b) {
	BigInteger ret = a;
	ret /= b;
	return ret;
}

BigInteger operator+(const BigInteger &a, const BigInteger& b) {
	BigInteger ret = a;
	ret += b;
	return ret;
}

BigInteger operator-(const BigInteger &a, const BigInteger& b) {
		BigInteger ret = a;
		ret -= b;
		return ret;
}

BigInteger operator *(const BigInteger &a, const BigInteger& b) {
	BigInteger ret = a;
	ret *= b;
	return ret;
}
BigInteger operator *(const BigInteger &a, const uint32 b) {
	BigInteger ret = a;
	ret *= b;
	return ret;
}

BigInteger BigInteger::pow(uint32 n) {
	BigInteger ret;
	ret.data[0] = 1;
	BigInteger t = (*this);
	while (n) {
		if (n & 1) {ret *= t, --n;}
		else { t *= t; n >>= 1;}
	}	
	return ret;
}

BigInteger BigInteger::powMod(uint32 n, const BigInteger &mod) {
	BigInteger ret;
	ret.data[0] = 1;
	BigInteger t = (*this);
	while (n) {
		if (n & 1) { (ret *= t) %= mod, --n;}
		else { (t *= t ) %= mod; n >>= 1;}
	}	
	return ret;	
}
