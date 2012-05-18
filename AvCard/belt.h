#ifndef __BELT_H
#define __BELT_H
#include "typedef.h"

#define BELT_MAC_LENGHT 8
#define BELT_SYNCHRO_LENGHT 16
#define BELT_HASH_LENGHT 32
#define BELT_KEY_LENGHT 32

static const byte H[1 << 8];

template<int R> uint32 RotHi(uint32 u) {
	return (u << R) | (u >> (32 - R));
}
template<int R> uint32 RotLo(uint32 u) {
	return (u >> R) | (u << (32 - R));
}
template<int R> uint32 G(uint32 u) {
	uint32 ret = 0, t;
	ret |= (t = H[u&0xFF]);
	u >>= 8;
	ret |= (t = H[u&0xFF])<<8;
	u >>= 8;
	ret |= (t = H[u&0xFF])<<16;
	u >>= 8;
	ret |= (t = H[u&0xFF])<<24;
	return RotHi<R>(ret);
}

uint32 plus(const uint32& a, const uint32 &b) { return a + b; }
uint32 minus(const uint32&a, const uint32 &b) {
	if (a >= b) return a - b;
	else {
		uint64 t = (1ULL) << 32;
		t += a;
		t -= b;
		return t;
	}
}
uint32 eval(uint32 r) { return (r - 1)& 7; }
void encrypt_block(uint32* X, uint32 *Y, uint32 *sigma) {
	uint32 a = *X, b=*(X + 1), c= *(X + 2), d = *(X + 3),e;
	for (uint32 i = 1; i<= 8; ++i) {
		b ^= G<5>(plus(a, *(sigma + eval(7*i - 6))));
		c ^= G<21>(plus(d, *(sigma + eval(7*i-5))));
		a = minus(a, G<13>(plus(b, *(sigma + eval(7*i - 4)))));
		e =G<21>(plus(plus(b,c), *(sigma + eval(7 * i - 3)))) ^ i;
		b=plus(b,e);
		c=minus(c,e);
		d = plus(d, G<13>(plus(c, *(sigma + eval(7*i-2)))));
		b ^= G<21>(plus(a, *(sigma + eval(7 * i - 1))));
		c ^= G<5>(plus(d, *(sigma + eval(7*i))));
		a ^=b, b^=a, a^=b;
		c ^=d, d ^=c, c ^=d;
		b ^=c, c ^= b, b ^= c;
	}
	*Y = b;
	*(Y + 1) = d;
	*(Y + 2) = a;
	*(Y + 3) = c;
}

void decrypt_block(uint32* X, uint32 *Y, uint32 *sigma) {
	uint32 a = *X, b=*(X + 1), c= *(X + 2), d = *(X + 3),e;
	for (uint32 i = 8; i>= 1; --i) {
		b ^= G<5>(plus(a, *(sigma + eval(7*i))));
		c ^= G<21>(plus(d, *(sigma + eval(7*i-1))));
		a = minus(a, G<13>(plus(b, *(sigma + eval(7*i - 2)))));
		e =G<21>(plus(plus(b,c), *(sigma + eval(7 * i - 3)))) ^ i;
		b=plus(b,e);
		c=minus(c,e);
		d = plus(d, G<13>(plus(c, *(sigma + eval(7*i-4)))));
		b ^= G<21>(plus(a, *(sigma + eval(7 * i - 5))));
		c ^= G<5>(plus(d, *(sigma + eval(7*i-6))));
		a ^=b, b^=a, a^=b;
		c ^=d, d ^=c, c ^=d;
		a ^=d, d ^= a, a ^= d;
	}
	*Y = c;
	*(Y + 1) = a;
	*(Y + 2) = d;
	*(Y + 3) = b;
}



EXTERN_C void belt_ecb_ecnr(byte *XX, uint32 size, byte *Sigma, byte *to) {
	//size in bytes
	uint32 act_sz = ((size - 1) / 16 + 1) * 4;
	uint32 *X = new uint32[act_sz];
	uint32 *Y = new uint32[act_sz];
	uint32 byteSZ = act_sz << 2;
	uint32* sigma = (uint32*)sigma;
	memcpy(X, XX, size); 
	//here goes block with r
	if (size   & 15 == 0) {
		for (size_t i = 0; i < act_sz; i += 4) {
			encrypt_block(X  + i, Y + i,sigma);
		}
	} else {
		for (size_t i = 0; i < act_sz - 8; i += 4) {
			encrypt_block(X  + i, Y + i, sigma);
		}
		encrypt_block(X + (act_sz - 8), Y + act_sz - 4, sigma);
		uint32 diff = byteSZ - size;
		memcpy(((byte*)X) + size, ((byte*)Y) + size, diff);
		encrypt_block(X + (act_sz - 4), Y + (act_sz - 8), sigma);
	}
	memcpy(to, ((byte*)Y), size);
	delete X;
	delete Y;
}

EXTERN_C void belt_ecb_decr(byte *XX, uint32 size, byte *Sigma, byte *to){ 
	//size in bytes
	uint32 act_sz = ((size - 1) / 16 + 1) * 4;
	uint32 *X = new uint32[act_sz];
	uint32 *Y = new uint32[act_sz];
	uint32 byteSZ = act_sz << 2;
	uint32* sigma = (uint32*)sigma;
	memcpy(X, XX, size); 
	//here goes block with r
	if (size   & 15 == 0) {
		for (size_t i = 0; i < act_sz; i += 4) {
			decrypt_block(X  + i, Y + i,sigma);
		}
	} else {
		for (size_t i = 0; i < act_sz - 8; i += 4) {
			decrypt_block(X  + i, Y + i, sigma);
		}
		decrypt_block(X + (act_sz - 8), Y + act_sz - 4, sigma);
		uint32 diff = byteSZ - size;
		memcpy(((byte*)X) + size, ((byte*)Y) + size, diff);
		decrypt_block(X + (act_sz - 4), Y + (act_sz - 8), sigma);
	}
	memcpy(to, ((byte*)Y), size);
	delete X;
	delete Y;
}


EXTERN_C void belt_ctr(byte *XX, uint32 size, byte *Sigma, byte *S, byte *to){
	uint32 ss[4], ss2[4];
	uint32 *sigma = (uint32*)sigma;
	encrypt_block((uint32*)S, ss, sigma);
	uint32 act_sz = ((size - 1) / 16 + 1) * 4;
	uint32 *X = new uint32[act_sz];
	uint32 *Y = new uint32[act_sz];
	uint32 byteSZ = act_sz << 2;
	memcpy(X, XX, size);
	BigInteger s((byte*)ss, 16);
	BigInteger one(1);
	for (size_t i = 0; i < act_sz; i += 4) {
		s += one;
		s.reduce(4);
		for (size_t j = 0; j < 4; ++j) ss2[j] = s.data[3 - j];
		encrypt_block(ss2, ss, sigma);
		for (size_t j = 0; j < 4; ++j) Y[i + j] = X[i + j] ^ ss[j];
	}
	memcpy(to, Y, size);
	delete X;
	delete Y;
}

EXTERN_C void belt_mac(byte *X, uint32 size, byte *Sigma, byte *to);

EXTERN_C void belt_hash(byte *X, uint32 size, byte *to);

EXTERN_C void belt_hash_step(byte *X, byte *S, byte *to);

EXTERN_C void belt_keyrep(byte *X, byte b, byte *to);



#endif