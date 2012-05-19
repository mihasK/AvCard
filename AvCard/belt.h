#pragma once
#include "bigint.h"

#define BELT_MAC_LENGHT 8
#define BELT_SYNCHRO_LENGHT 16
#define BELT_HASH_LENGHT 32
#define BELT_KEY_LENGHT 32
static const byte H[] = {0xB1,0x94,0xBA,0xC8,0x0A,0x08,0xF5,0x3B,0x36,0x6D,0x00,0x8E,0x58,0x4A,0x5D,0xE4,
0x85,0x04,0xFA,0x9D,0x1B,0xB6,0xC7,0xAC,0x25,0x2E,0x72,0xC2,0x02,0xFD,0xCE,0x0D,
0x5B,0xE3,0xD6,0x12,0x17,0xB9,0x61,0x81,0xFE,0x67,0x86,0xAD,0x71,0x6B,0x89,0x0B,
0x5C,0xB0,0xC0,0xFF,0x33,0xC3,0x56,0xB8,0x35,0xC4,0x05,0xAE,0xD8,0xE0,0x7F,0x99,
0xE1,0x2B,0xDC,0x1A,0xE2,0x82,0x57,0xEC,0x70,0x3F,0xCC,0xF0,0x95,0xEE,0x8D,0xF1,
0xC1,0xAB,0x76,0x38,0x9F,0xE6,0x78,0xCA,0xF7,0xC6,0xF8,0x60,0xD5,0xBB,0x9C,0x4F,
0xF3,0x3C,0x65,0x7B,0x63,0x7C,0x30,0x6A,0xDD,0x4E,0xA7,0x79,0x9E,0xB2,0x3D,0x31,
0x3E,0x98,0xB5,0x6E,0x27,0xD3,0xBC,0xCF,0x59,0x1E,0x18,0x1F,0x4C,0x5A,0xB7,0x93,
0xE9,0xDE,0xE7,0x2C,0x8F,0x0C,0x0F,0xA6,0x2D,0xDB,0x49,0xF4,0x6F,0x73,0x96,0x47,
0x06,0x07,0x53,0x16,0xED,0x24,0x7A,0x37,0x39,0xCB,0xA3,0x83,0x03,0xA9,0x8B,0xF6,
0x92,0xBD,0x9B,0x1C,0xE5,0xD1,0x41,0x01,0x54,0x45,0xFB,0xC9,0x5E,0x4D,0x0E,0xF2,
0x68,0x20,0x80,0xAA,0x22,0x7D,0x64,0x2F,0x26,0x87,0xF9,0x34,0x90,0x40,0x55,0x11,
0xBE,0x32,0x97,0x13,0x43,0xFC,0x9A,0x48,0xA0,0x2A,0x88,0x5F,0x19,0x4B,0x09,0xA1,
0x7E,0xCD,0xA4,0xD0,0x15,0x44,0xAF,0x8C,0xA5,0x84,0x50,0xBF,0x66,0xD2,0xE8,0x8A,
0xA2,0xD7,0x46,0x52,0x42,0xA8,0xDF,0xB3,0x69,0x74,0xC5,0x51,0xEB,0x23,0x29,0x21,
0xD4,0xEF,0xD9,0xB4,0x3A,0x62,0x28,0x75,0x91,0x14,0x10,0xEA,0x77,0x6C,0xDA,0x1D
};

static void phi1(uint32* u) {
	uint32 t = u[0]^u[1];
	for (size_t i = 2; i >=0; --i) u[i] = u[i + 1];
	u[3] = t;
}

static void phi2(uint32* u) {
	uint32 t = u[0]^u[3];
	for (size_t i = 1; i <= 3; ++i) u[i] = u[i - 1];
	u[0] = t;
}
static void psi(uint32* u, uint32 at) {
	if (at  < 4) {
		u[at] =(1U) << 31;
	}
}


static void change_endian(byte *X) {
	byte*l = X, 
			*r = X + 3;

		*r ^= *l, *l ^= *r, *r ^= *l;
		l = X + 1, 
			r = X + 2;

		*r ^= *l, *l ^= *r, *r ^= *l;

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
	change_endian((byte*)&ret);
	ret = RotHi<R>(ret);
		change_endian((byte*)&ret);
		return ret;
}


static uint32 plus_belt(const uint32& a, const uint32 &b) {
	uint32 aa = a;
	change_endian((byte*)&aa);
	uint32 bb = b;
	change_endian((byte*)&bb);
	uint32 tt = aa + bb;
	change_endian((byte*)&tt);
	return tt;
}


static uint32 minus_belt(const uint32&a, const uint32 &b) {
	uint32 aa = a;
	change_endian((byte*)&aa);
	uint32 bb = b;
	change_endian((byte*)&bb);
	uint32 tt;
	if (aa >= bb) tt = aa - bb;
	else {
		uint64 t = (1ULL) << 32;
		t += aa;
		t -= bb;
		tt= t;
	}
	change_endian((byte*)&tt);
	return tt;
}
static uint32 eval(uint32 r) {
	return (r - 1)& 7;
}
static void encrypt_block(uint32* X, uint32 *Y, uint32 *sigma) {
	uint32 a = *X, b=*(X + 1), c= *(X + 2), d = *(X + 3),e;
	for (uint32 i = 1; i<= 8; ++i) {
		b ^= G<5>(plus_belt(a, *(sigma + eval(7*i - 6))));
		c ^= G<21>(plus_belt(d, *(sigma + eval(7*i-5))));
		a = minus_belt(a, G<13>(plus_belt(b, *(sigma + eval(7*i - 4)))));
		uint32 t_i = i;
		change_endian((byte*)&t_i);
		e =G<21>(plus_belt(plus_belt(b,c), *(sigma + eval(7 * i - 3)))) ^ t_i;
		b=plus_belt(b,e);
		c=minus_belt(c,e);
		d = plus_belt(d, G<13>(plus_belt(c, *(sigma + eval(7*i-2)))));
		b ^= G<21>(plus_belt(a, *(sigma + eval(7 * i - 1))));
		c ^= G<5>(plus_belt(d, *(sigma + eval(7*i))));
		a ^=b, b^=a, a^=b;
		c ^=d, d ^=c, c ^=d;
		b ^=c, c ^= b, b ^= c;
	}
	*Y = b;
	*(Y + 1) = d;
	*(Y + 2) = a;
	*(Y + 3) = c;
}

static void decrypt_block(uint32* X, uint32 *Y, uint32 *sigma) {
	uint32 a = *X, b=*(X + 1), c= *(X + 2), d = *(X + 3),e;
	for (uint32 i = 8; i>= 1; --i) {
		b ^= G<5>(plus_belt(a, *(sigma + eval(7*i))));
		c ^= G<21>(plus_belt(d, *(sigma + eval(7*i-1))));
		a = minus_belt(a, G<13>(plus_belt(b, *(sigma + eval(7*i - 2)))));
		uint32 t_i = i;
		change_endian((byte*)&t_i);
		e =G<21>(plus_belt(plus_belt(b,c), *(sigma + eval(7 * i - 3)))) ^ t_i;
		b=plus_belt(b,e);
		c=minus_belt(c,e);
		d = plus_belt(d, G<13>(plus_belt(c, *(sigma + eval(7*i-4)))));
		b ^= G<21>(plus_belt(a, *(sigma + eval(7 * i - 5))));
		c ^= G<5>(plus_belt(d, *(sigma + eval(7*i-6))));
		a ^=b, b^=a, a^=b;
		c ^=d, d ^=c, c ^=d;
		a ^=d, d ^= a, a ^= d;
	}
	*Y = c;
	*(Y + 1) = a;
	*(Y + 2) = d;
	*(Y + 3) = b;
}
 static void belt_ecb_encr(byte *XX, uint32 size, byte *Sigma, byte *to) {
	//size in bytes
	uint32 act_sz = ((size - 1) / 16 + 1) * 4;
	uint32 *X = new uint32[act_sz];
	uint32 *Y = new uint32[act_sz];
	uint32 byteSZ = act_sz << 2;
	uint32* sigma = (uint32*)Sigma;
	memcpy(X, XX, size);
	for (size_t i = 0; i < byteSZ; i += 4) {
		byte*l = ((byte*)X) + i, 
			*r = ((byte*)X) + 3 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
		l = ((byte*)X) + i + 1, 
			r = ((byte*)X) + 2 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
	}
	for (size_t i = 0; i < 32; i += 4) {
		byte*l = ((byte*)sigma) + i, 
			*r = ((byte*)sigma) + 3 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
		l = ((byte*)sigma) + i + 1, 
			r = ((byte*)sigma) + 2 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
	}
	//here goes block with r
	if ((size   & 15) == 0) {
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
	for (int i = 0; i < act_sz; ++i) {
		change_endian((byte*)(Y + i));
	}
	for (size_t i = 0; i < 32; i += 4) {
		byte*l = ((byte*)sigma) + i, 
			*r = ((byte*)sigma) + 3 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
		l = ((byte*)sigma) + i + 1, 
			r = ((byte*)sigma) + 2 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
	}
	memcpy(to, ((byte*)Y), size);
	delete X;
	delete Y;
}

 static void belt_ecb_decr(byte *XX, uint32 size, byte *Sigma, byte *to){ 
	//size in bytes
	uint32 act_sz = ((size - 1) / 16 + 1) * 4;
	uint32 *X = new uint32[act_sz];
	uint32 *Y = new uint32[act_sz];
	uint32 byteSZ = act_sz << 2;
	uint32* sigma = (uint32*)Sigma;
		memcpy(X, XX, size); 
	for (size_t i = 0; i < byteSZ; i += 4) {
		byte*l = ((byte*)X) + i, 
			*r = ((byte*)X) + 3 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
		l = ((byte*)X) + i + 1, 
			r = ((byte*)X) + 2 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
	}
	for (size_t i = 0; i < 32; i += 4) {
		byte*l = ((byte*)sigma) + i, 
			*r = ((byte*)sigma) + 3 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
		l = ((byte*)sigma) + i + 1, 
			r = ((byte*)sigma) + 2 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
	}

	//here goes block with r
	if ((size   & 15) == 0) {
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
	for (int i = 0; i < act_sz; ++i) {
		change_endian((byte*)(Y + i));
	}
	for (size_t i = 0; i < 32; i += 4) {
		byte*l = ((byte*)sigma) + i, 
			*r = ((byte*)sigma) + 3 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
		l = ((byte*)sigma) + i + 1, 
			r = ((byte*)sigma) + 2 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
	}
	memcpy(to, ((byte*)Y), size);
	delete X;
	delete Y;
}


static  void belt_ctr(byte *XX, uint32 size, byte *Sigma, byte *S, byte *to){
	uint32 ss[4], ss2[4];
	uint32 *sigma = (uint32*)Sigma;
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

static void belt_mac(byte *XX, uint32 size, byte *Sigma, byte *to) {
	uint32* sigma = (uint32*)Sigma;
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

static void sigma1(uint32 *U, uint32 *u){
	uint32 u34[4];
	for (size_t i =0; i < 4; ++i) u34[i] = U[8 + i] ^ U[12 + i];
	encrypt_block(u34, u, U);
	for (size_t i = 0; i < 4; ++i) u[i] ^= u34[i];
}

static void sigma2(uint32 * U, uint32 *u) {
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


static  void belt_hash_step(byte* XX, uint32 cur_len, byte *STATE, byte *to = NULL) {
	uint32 s[4];
	memset(s, 0, sizeof s);
	uint32 act_sz = 8;
	uint32 h[8] = {0x0DCEFD02, 0xC2722E25, 0xACC7B61B, 0x9DFA0485, 0xE45D4A58, 0x8E006D36, 0x3BF5080A,0xC8BA94B1};	
	if (cur_len == 0 && XX != NULL) {
		memcpy(STATE + 16, s, sizeof s);
		memcpy(STATE + 16 + sizeof s, h, sizeof h);
		memset(STATE , 0, 16);
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

 static void belt_hash(byte *XX, uint32 size, byte *to) {

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

 static void belt_keyrep(byte *X, byte b, byte *to) {
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


