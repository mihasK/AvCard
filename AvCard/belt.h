#ifndef __BELT_H
#define __BELT_H
#include "typedef.h"
#include "bigint.h"

#define BELT_MAC_LENGHT 8
#define BELT_SYNCHRO_LENGHT 16
#define BELT_HASH_LENGHT 32
#define BELT_KEY_LENGHT 32

static const byte H[1 << 8];

void phi1(uint32* u) {
	uint32 t = u[0]^u[1];
	for (size_t i = 2; i >=0; --i) u[i] = u[i + 1];
	u[3] = t;
}

void phi2(uint32* u) {
	uint32 t = u[0]^u[3];
	for (size_t i = 1; i <= 3; ++i) u[i] = u[i - 1];
	u[0] = t;
}
void psi(uint32* u, uint32 at) {
	if (at  < 4) {
		u[at] =(1U) << 31;
	}
}




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

EXTERN_C void belt_mac(byte *XX, uint32 size, byte *Sigma, byte *to) {
	uint32* sigma = (uint32*)sigma;
	uint32 act_sz = ((size - 1) / 16 + 1) * 4;
	uint32 *X = new uint32[act_sz];
	uint32 *Y = new uint32[act_sz];
	uint32 byteSZ = act_sz << 2;
	memcpy(X, XX, size);
	uint32 s[4], r[4];
	memset(s, 0, sizeof s);
	encrypt_block(s, r, sigma);

	for (size_t i = 0; i < act_sz - 4; i += 4) {
		for (size_t j = 0; j < 4; ++j) X[i + j] ^= s[j];
		encrypt_block(X + i, s, sigma);
	}
	uint32 diff = byteSZ - size;
	if (!diff) {
		phi1(r);
		for (size_t i = 0; i < 4; ++i) s[i] ^= r[i] ^ X[act_sz - 4 + i];
	} else {
		psi(X + act_sz - 4, 4 - diff);
		phi2(r);
		for (size_t i = 0; i < 4; ++i) s[i] ^= r[i] ^ X[act_sz - 4 + i];
	}
	encrypt_block(s, r, sigma);
	memcpy(to, r, 8);
	delete X;
	delete Y;
}

void sigma1(uint32 *U, uint32 *u){
	uint32 u34[4];
	for (size_t i =0; i < 4; ++i) u34[i] = U[8 + i] ^ U[12 + i];
	encrypt_block(u34, u, U);
	for (size_t i = 0; i < 4; ++i) u[i] ^= u34[i];
}

void sigma2(uint32 * U, uint32 *u) {
	uint32 SIGMA1[8], SIGMA2[8];
	sigma1(U, SIGMA1);
	for (size_t i = 0; i < 4; ++i) SIGMA1[4 + i] = U[12 + i];
	sigma1(U, SIGMA2);
	for (size_t i = 0; i < 4; ++i) SIGMA2[4 + i] = U[8 + i], SIGMA2[i] ^= (uint32)(((1ULL)<<32) - 1);
	encrypt_block(U, u, SIGMA1);
	for (size_t i = 0; i < 4; ++i) u[i] ^= U[i];
	encrypt_block(U + 4, u + 4, SIGMA2);
	for (size_t i = 0; i < 4; ++i) u[4 + i] ^= U[4 + i];
}


EXTERN_C void belt_hash_step(byte* XX, uint32 cur_len, byte *&STATE, byte *to = NULL) {
	uint32 s[4];
	memset(s, 0, sizeof s);
	uint32 act_sz = 8;
	uint32 h[8] = {0x0DCEFD02, 0xC2722E25, 0xACC7B61B, 0x9DFA0485, 0xE45D4A58, 0x8E006D36, 0x3BF5080A,0xC8BA94B1};	
	if (cur_len == 0) {
		memcpy(STATE + 16, s, sizeof s);
		memcpy(STATE + 16 + sizeof s, h, sizeof h);
		memset(STATE + 16, 0, 16);
		return;
	} else {
		if (XX == NULL) {
			uint32 tmp3[8];
			sigma2((uint32*)STATE, tmp3);
			memcpy(to, tmp3, 32);
			return;
		}
		uint32 *X = new uint32[act_sz];
		uint32 byteSZ = act_sz << 2;
		memcpy(X, XX, cur_len);
		uint32 tmp1[4], tmp2[8], tmp3[16];
		memcpy(s, STATE + 16, sizeof s);
		memcpy(h, STATE + sizeof s + 16, sizeof h);
		uint64 X_LEN;
		memcpy(&X_LEN, STATE + 8, 8);
		X_LEN += cur_len;
		for (size_t j = 0; j < 8; ++j) tmp3[j] = X[j], tmp3[8 + j] = h[j];
		sigma1(tmp3, tmp1);
		for (size_t j = 0; j < 4; ++j) s[j] ^= tmp1[j];
		sigma2(tmp3, h);
		memcpy(STATE + 16, s, sizeof s);
		memcpy(STATE + 16 + sizeof s, h, sizeof h);
		memcpy(STATE + 8, &X_LEN, 8);
	}
}

EXTERN_C void belt_hash(byte *XX, uint32 size, byte *to) {
	belt_hash_step(XX, 0, to);
	uint32 actSIZE = 0;
	while (actSIZE + 8 < size) {
		belt_hash_step(XX + actSIZE, 8, to);
		actSIZE += 8;
	}
	size-=actSIZE;
	belt_hash_step(XX + actSIZE, size, to, to);
	belt_hash_step(NULL, 0, to);
}

EXTERN_C void belt_keyrep(byte *X, byte b, byte *to) {
	uint32 n = 256, m = 256;
	byte D[12];
	memset(D, 0, sizeof D);
	byte I[16];
	memset(I, 0, sizeof I);
	I[0] = b;
	uint32 r = 0x7B653CF3;
	uint32 preY[16];
	preY[0] = r;
	memcpy(preY + 1, D, sizeof D);
	memcpy(preY + 4, I, sizeof I);
	memcpy(preY + 8, X, 32);
	uint32 Y[8];
	sigma2(preY, Y);
	memcpy(to, Y, sizeof Y);
}



#endif