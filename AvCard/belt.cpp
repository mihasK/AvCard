#include "belt.h"

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
	uint32 _=at>>2;
	uint32 __=at&3;
	__=(3-__)&3;
	*(((byte*)(u+_))+__)=0x80;
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
		uint32 diff = byteSZ - size; //in bytes
		uint32 diff2 = diff / 4 ; //in ints
		uint32 at = (size - 1) >> 1;
		for (size_t jj = 0; jj<diff2; ++jj) {
			X[at+jj] = Y[at+jj];
		}
		for (size_t jj = 0; jj < diff; ++jj) {
			*(((byte*)(X + act_sz - 1)) + jj) = *(((byte*)(Y + act_sz - 1)) + jj);
		}
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
		uint32 diff = byteSZ - size; //in bytes
		uint32 diff2 = diff / 4 ; //in ints
		uint32 at = (size - 1) >> 1;
		for (size_t jj = 0; jj<diff2; ++jj) {
			X[at+jj] = Y[at+jj];
		}
		for (size_t jj = 0; jj < diff; ++jj) {
			*(((byte*)(X + act_sz - 1)) + jj) = *(((byte*)(Y + act_sz - 1)) + jj);
		}
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

	for (size_t i = 0; i < 32; i += 4) {
		byte*l = ((byte*)sigma) + i, 
			*r = ((byte*)sigma) + 3 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
		l = ((byte*)sigma) + i + 1, 
			r = ((byte*)sigma) + 2 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
	}

	uint32 act_sz = ((size - 1) / 16 + 1) * 4;
	uint32 *X = new uint32[act_sz];
	uint32 *Y = new uint32[act_sz];
	uint32 byteSZ = act_sz << 2;
	memset(X, 0U, sizeof X);
	memcpy(X, XX, size);
	for (size_t i = 0; i < byteSZ; i += 4) {
		byte*l = ((byte*)X) + i, 
			*r = ((byte*)X) + 3 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
		l = ((byte*)X) + i + 1, 
			r = ((byte*)X) + 2 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
	}
	uint32 S2[4];
	memcpy(S2, S, 16);
	for (size_t jj = 0; jj < 4; ++jj) change_endian((byte*)(S2 + jj));
	encrypt_block(S2, ss, sigma);	
	byte hlp[16];
	memcpy(hlp, ss, sizeof hlp);
	for (size_t jj = 0; jj < 16; jj += 4) change_endian(hlp+jj);

	BigInteger s;
	s.length = 4;
	for (size_t jj =0 ; jj < 4; ++jj) s.data[jj] = ss[jj], change_endian((byte*)(s.data + jj));
	BigInteger one(1);
	change_endian((byte*)(one.data));
	for (size_t i = 0; i < act_sz; i += 4) {
		s += one;
		s.reduce(4);
		for (size_t j = 0; j < 4; ++j) ss2[j] = s.data[j],change_endian((byte*)(ss2 + j));;
		encrypt_block(ss2, ss, sigma);
		for (size_t j = 0; j < 4; ++j) Y[i + j] = X[i + j] ^ ss[j];
	}
	for (int i = 0; i < act_sz; ++i) {
		change_endian((byte*)(Y + i));
	}
	memcpy(to, Y, size);


	for (size_t i = 0; i < 32; i += 4) {
		byte*l = ((byte*)sigma) + i, 
			*r = ((byte*)sigma) + 3 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
		l = ((byte*)sigma) + i + 1, 
			r = ((byte*)sigma) + 2 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
	}

	delete X;
	delete Y;
}

static void belt_mac(byte *XX, uint32 size, byte *Sigma, byte *to) {
	uint32* sigma = (uint32*)Sigma;
	uint32 act_sz = ((size - 1) / 16 + 1) * 4;
	uint32 *X = new uint32[act_sz];
	uint32 *Y = new uint32[act_sz];
	uint32 byteSZ = act_sz << 2;
	memset(X, 0x00, sizeof X);
	for (size_t jj = 0; jj < act_sz; ++jj) X[jj] ^= X[jj];
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
		psi(X + act_sz - 4,16- diff);
		phi2(r);
		for (size_t i = 0; i < 4; ++i) s[i] ^= r[i] ^ X[act_sz - 4 + i];
	}
	encrypt_block(s, r, sigma);
	for (size_t jj = 0; jj < 4; ++jj) change_endian((byte*)(r + jj));
	memcpy(to, r, 8);
	for (size_t i = 0; i < 32; i += 4) {
		byte*l = ((byte*)sigma) + i, 
			*r = ((byte*)sigma) + 3 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
		l = ((byte*)sigma) + i + 1, 
			r = ((byte*)sigma) + 2 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
	}
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


static  void belt_hash_step(byte* XX, uint32 cur_len, byte *STATE, byte *to) {
	uint32 s[4];
	memset(s, 0, sizeof s);
	uint32 act_sz = 8;
	byte  h[32] = { 0xB1 ,0x94 ,0xBA ,0xC8 ,0x0A ,0x08 ,0xF5 ,0x3B ,0x36 ,0x6D ,0x00 ,0x8E ,0x58 ,0x4A ,0x5D ,0xE4 ,0x85 ,0x04 ,0xFA ,0x9D ,0x1B ,0xB6 ,0xC7 ,0xAC ,0x25 ,0x2E ,0x72 ,0xC2 ,0x02 ,0xFD ,0xCE ,0x0D};
	if (cur_len == 0 && XX != NULL) {
		memcpy(STATE + 16, s, sizeof s);
		memcpy(STATE + 16 + sizeof s, h, sizeof h);
		memset(STATE , 0, 16);
		return;
	} else {
		if (XX == NULL) {
			uint32 tmp3[8];
			uint32 tmp1[16];
			memcpy(tmp1, STATE, sizeof tmp1);
			for (size_t  i = 0; i < 16; ++i) change_endian((byte*)(tmp1 + i));
			sigma2(tmp1, tmp3);
			memcpy(to, tmp3, 32);
			return;
		}
		uint32 *X = new uint32[act_sz];
		uint32 byteSZ = act_sz << 2;
		memcpy(X, XX, cur_len);


		memcpy(s, STATE + 16, sizeof s);
		memcpy(h, STATE + sizeof s + 16, sizeof h);
		for (size_t i = 0; i < byteSZ; i += 4) {
			byte*l = ((byte*)X) + i, 
				*r = ((byte*)X) + 3 + i;

			*r ^= *l, *l ^= *r, *r ^= *l;
			l = ((byte*)X) + i + 1, 
				r = ((byte*)X) + 2 + i;

			*r ^= *l, *l ^= *r, *r ^= *l;
		}
		for (size_t i = 0; i < 16; i += 4) {
			byte*l = ((byte*)s) + i, 
				*r = ((byte*)s) + 3 + i;

			*r ^= *l, *l ^= *r, *r ^= *l;
			l = ((byte*)s) + i + 1, 
				r = ((byte*)s) + 2 + i;

			*r ^= *l, *l ^= *r, *r ^= *l;
		}
		uint64 X_LEN;
		memcpy(&X_LEN, STATE + 8, 8);
		X_LEN += cur_len << 2;
		uint32 tmp1[4], tmp2[8], tmp3[16];
		for (size_t j = 0; j < 8; ++j) tmp3[j] = X[j];
		for (size_t j = 0; j < 32; ++j) *(((byte*)(tmp3+8)) + j) = h[j];
		sigma1(tmp3, tmp1);
		for (size_t j = 0; j < 4; ++j) s[j] ^= tmp1[j];
		sigma2(tmp3, tmp2);
		for (size_t j = 0; j < 8; ++j) change_endian((byte*)(tmp2 + j));
		memcpy(STATE + 16, s, sizeof s);
		memcpy(STATE + 16 + sizeof s, tmp2, sizeof h);
		memcpy(STATE + 8, &X_LEN, 8);
	}
}

static void belt_hash(byte *XX, uint32 size, byte *to) {
	byte STATE[64];
	belt_hash_step(XX, 0,STATE);
	uint32 actSIZE = 0;
	while (actSIZE + 8 < size) {
		belt_hash_step(XX + actSIZE, 8, STATE);
		actSIZE += 8;
	}
	size-=actSIZE;
	belt_hash_step(XX + actSIZE, size, STATE, to);
	belt_hash_step(NULL, 0, STATE, to);
}

static void belt_keyrep(byte *X, byte b, byte *to) {
	uint32 n = 256, m = 256;
	byte D[12];
	memset(D, 0, sizeof D);
	byte I[16];
	memset(I, 0, sizeof I);
	I[0] = b;
	uint32 r = 0x7B653CF3;
	change_endian((byte*)&r);
	uint32 preY[16];
	preY[0] = r;
	memcpy(preY + 1, D, sizeof D);
	memcpy(preY + 4, I, sizeof I);
	memcpy(preY + 8, X, 32);
	for (size_t jj = 1; jj < 16; ++jj) change_endian((byte*)(preY +jj));
	uint32 Y[8];
	sigma2(preY, Y);
	memcpy(to, Y, sizeof Y);
}

static void belt_keywrap(byte *X, uint32 len, byte *Sigma, byte *to) {
	uint32 *r = new uint32[ (sizeof X - 1) / 4 + 1 + 16];
	memset(r, 0x00, sizeof r);
	memcpy(r, X, sizeof X);
	for (uint32 jj =0 ; jj < ((sizeof X - 1) / 4 + 1); ++jj) change_endian((byte*)(r + jj));
	uint32 s[4];
	uint32 *sigma = (uint32*)Sigma;

	for (size_t i = 0; i < 32; i += 4) {
		byte*l = ((byte*)sigma) + i, 
			*r = ((byte*)sigma) + 3 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
		l = ((byte*)sigma) + i + 1, 
			r = ((byte*)sigma) + 2 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
	}

	uint32 n = sizeof r >> 2;
	for (uint32 step = 1; step <= (n << 1); ++step) {
		for (uint32 aa=0;aa<4;aa++) {
			s[aa] = 0x00000000;
			for (uint32 bb=aa;bb<n - 4;bb+=4)s[aa]^=r[bb];
		}
		uint32 h[4];
		encrypt_block(s, h, sigma);
		for (size_t qq=0;qq<4;++qq) r[n - 4 + qq] ^= h[qq]^ step;
		for (size_t jj=4;jj<n;++jj)r[jj-4]=r[jj];
		for (size_t jj=0;jj<4;++jj)r[n-4+jj]=s[jj];
	}
	for (uint32 tt=0;tt<n;++tt) change_endian((byte*)(r+tt));
	memcpy(to, r, sizeof r);
	delete r;
}

static void belt_keyuwrap(byte *X,uint32 len,  byte *Sigma, byte *to) {
	uint32 *r = new uint32[ (sizeof X - 1) / 4 + 1 + 16];
	memset(r, 0x00, sizeof r);
	memcpy(r, X, sizeof X);
	for (uint32 jj =0 ; jj < ((sizeof X - 1) / 4 + 1); ++jj) change_endian((byte*)(r + jj));
	uint32 s[4];
	uint32 *sigma = (uint32*)Sigma;

	for (size_t i = 0; i < 32; i += 4) {
		byte*l = ((byte*)sigma) + i, 
			*r = ((byte*)sigma) + 3 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
		l = ((byte*)sigma) + i + 1, 
			r = ((byte*)sigma) + 2 + i;

		*r ^= *l, *l ^= *r, *r ^= *l;
	}

	uint32 n = sizeof r >> 2;
	for (uint32 step = n << 1; step <(n << 3); --step) {
		for (size_t jj=0;jj<4;++jj)s[jj] =r[n-4+jj];
		for (size_t jj=0;jj<n-4;++jj)r[jj+4]=r[jj];
		for (uint32 aa=0;aa<4;aa++) {
			s[aa] = 0x00000000;
			for (uint32 bb=aa;bb<n - 4;bb+=4)s[aa]^=r[bb];
		}
		uint32 h[4];
		encrypt_block(s, h, sigma);
		for (size_t qq=0;qq<4;++qq) r[n - 4 + qq] = h[qq]^ step;
		for (uint32 aa=0;aa<4;aa++) {
			r[aa] = s[aa];
			for (uint32 bb=aa + 4;bb<n - 4;bb+=4)r[aa]^=r[bb];
		}
	}
	for (uint32 tt=0;tt<n;++tt) change_endian((byte*)(r+tt));
	memcpy(to, r, sizeof r);
	delete r;
}


