#pragma once
#include "bign.h"

static void PI(byte *A, Point & b) {
	uint32 ARRAY_OFFSET = 0x00;
	byte PI_HELPER[1 << 2] = {0x00};
	for (size_t jj = 0; jj < 8; ++jj) {
		memcpy(PI_HELPER, b.x.data + jj, 4);
		change_endian(PI_HELPER);
		for (size_t ll = 0; ll < 4; ++ll) A[ARRAY_OFFSET + (jj << 2) + ll] = PI_HELPER[ll];
	}
	ARRAY_OFFSET += 1 << 4;
	for (size_t jj = 0; jj < 8; ++jj) {
		memcpy(PI_HELPER, b.y.data + jj, 4);
		change_endian(PI_HELPER);
		for (size_t ll = 0; ll < 4; ++ll) A[ARRAY_OFFSET + (jj << 2) + ll] = PI_HELPER[ll];
	}
}


static void REV_PI(byte *A, Point & b) {
	uint32 ARRAY_OFFSET = 0x00;
	byte PI_HELPER[1 << 2] = {0x00};
	for (size_t jj = 0; jj < 8; ++jj) {
		memcpy(PI_HELPER, A + ARRAY_OFFSET + (jj << 2), 4);
		change_endian(PI_HELPER);
		memcpy(b.x.data + jj, PI_HELPER, 4);
	}
	ARRAY_OFFSET += 1 << 4;
	for (size_t jj = 0; jj < 8; ++jj) {
		memcpy(PI_HELPER, A + ARRAY_OFFSET + (jj << 2), 4);
		change_endian(PI_HELPER);
		memcpy(b.y.data + jj, PI_HELPER, 4);
	}

	b.y.length = b.x.length = 8;
}

static BOOL bign_valpubkey(byte *Q){
	Point P;
	BigInteger _P = bign_curve256v1::getP();
	REV_PI(Q, P);
	if (P.x >= _P || P.y >= _P) return false;
	BigInteger left = (P.y * P.y) % _P;
	BigInteger right = ((P.x * P.x % _P) * P.x % _P + (bign_curve256v1::getA() * P.x) % _P + bign_curve256v1::getB()) % _P;
	return (left == right);
}

static void gen_rnd_data(byte *q, int len) {
	srand(time(NULL));
	for (size_t i=  0; i < len; ++i) q[i] = rand();
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

static void bign_sign(byte *H, byte *d, byte* to) {
	byte rand[32];
	gen_rnd_data(rand, 32);
	BigInteger k(rand, 32);
	BigInteger P = bign_curve256v1::getP();
	k %= P;
	byte dd[32];
	memcpy(dd, d, 32);
	for (size_t jj = 0; jj < 32; jj += 4) change_endian(dd + jj);
	BigInteger D(dd, 32);
	BigInteger Q = bign_curve256v1::getQ();
	Point G(BigInteger(0), bign_curve256v1::getY());
	Point R = doit(G, k);
	byte toHash[108];
	byte bR[64];
	PI(bR, R);
	memcpy(toHash, OID, sizeof OID);
	memcpy(toHash + sizeof OID, bR, sizeof bR);
	memcpy(toHash + sizeof OID + sizeof bR, H, 32);
	byte h_belt[32];
	belt_hash(toHash, sizeof toHash, h_belt);
	memcpy(to, h_belt, sizeof h_belt);
	for (size_t jj = 0; jj <32; jj += 4) change_endian(h_belt + jj);
	BigInteger temp1(h_belt, 32);
	temp1 <<= 128;
	temp1 %= Q;
	temp1 *= D %= Q;
	memcpy(h_belt, H, 32);
	for (size_t jj = 0; jj <32; jj += 4) change_endian(h_belt + jj);
	BigInteger temp2(h_belt, 32);
	temp2 %= Q;
	BigInteger temp3 = (k + Q - temp1 + Q - temp2) % Q;
	memcpy(h_belt, temp3.data, 32);
	for (size_t jj = 0; jj < 32; jj += 4) change_endian(h_belt + jj);
	memcpy(to + 32, h_belt, 32);
}
static void bign_prng(uint32 n, byte* Sigma, byte *S, byte *to){}
static  BOOL bign_verify(byte *H, byte *_q, byte *S, uint32 size){
	Point q;
	REV_PI(_q, q);
	BigInteger Q = bign_curve256v1::getQ();
	byte s0[32];
	memcpy(s0, S, sizeof s0);
	for (size_t jj = 0; jj < 32; jj += 4) change_endian(s0 + jj);
	BigInteger S0(s0, 32);
	S0 <<= 128;
	S0 %= Q;
	byte _qq[32];
	memcpy(_qq, S + 32, sizeof _qq);
	for (size_t jj = 0; jj < 32; jj += 4) change_endian(_qq + jj);
	BigInteger S1(_qq, 32);
	if (S1 >= Q) return false;
	byte h_belt[32];
	memcpy(h_belt, H, 32);
	for (size_t jj = 0; jj <32; jj += 4) change_endian(h_belt + jj);
	BigInteger temp2(h_belt, 32);
	temp2 %= Q;
	BigInteger rr = (temp2 + S1) % Q;
	BigInteger zero = BigInteger(0);
	Point G(zero, bign_curve256v1::getY());
	Point R = shamir(G, rr, q, S0);
	if (R.x == zero && R.y == zero) return false;
	byte toHash[108];
	byte bR[64];
	PI(bR, R);
	memcpy(toHash, OID, sizeof OID);
	memcpy(toHash + sizeof OID, bR, sizeof bR);
	memcpy(toHash + sizeof OID + sizeof bR, H, 32);
	belt_hash(toHash, sizeof toHash, h_belt);
	for (size_t jj = 0; jj < 32; ++jj) if (h_belt[jj] != S[jj]) return false;
	return true;
}
static void brng_hmac(byte *Sigma, byte *S, byte *to, uint32 size){
	uint32* sigma = (uint32*)Sigma;
	uint32 act_sz = ((size - 1) / 16 + 1) * 4;
	uint32 *X = new uint32[act_sz];
	uint32 *Y = new uint32[act_sz];
	uint32 byteSZ = act_sz << 2;
	memset(X, 0x00, sizeof X);
	for (size_t jj = 0; jj < act_sz; ++jj) X[jj] ^= X[jj];
	memcpy(X, S, size);
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
static void bign_keywrap(byte* X, uint32 len, byte *Q, byte *token){
	uint32 I[4];
	memset(I, 0x00, sizeof I);
	byte rand[32];
	gen_rnd_data(rand, 32);
	BigInteger k(rand, 32);
	BigInteger q = bign_curve256v1::getQ();
	k %= q;
	byte dd[32];
	Point _Q;
	REV_PI(Q, _Q);
	Point G(BigInteger(0), bign_curve256v1::getY());
	Point R = doit(G, k);
	Point _2 = doit(_Q, k);
	byte sigma[1<<6];
	PI(sigma, _2);
	PI(token, R);
	byte *hlp = new byte[len];
	belt_keywrap(X, len, sigma, hlp);
	memcpy(token+32, hlp, len);
	delete hlp;
}
static BOOL bign_keyunwrap(byte *X, byte *d, byte *untoken){
	Point q;
	REV_PI(X, q);
	BigInteger Q = bign_curve256v1::getQ();
	byte s0[32];
	memcpy(s0, d, sizeof s0);
	for (size_t jj = 0; jj < 32; jj += 4) change_endian(s0 + jj);
	BigInteger S0(s0, 32);
	S0 <<= 128;
	S0 %= Q;
	byte h_belt[32];
	memcpy(h_belt, H, 32);
	for (size_t jj = 0; jj <32; jj += 4) change_endian(h_belt + jj);
	BigInteger temp2(h_belt, 32);
	temp2 %= Q;
	byte _qq[32];
	memcpy(_qq, d + 32, sizeof _qq);
	for (size_t jj = 0; jj < 32; jj += 4) change_endian(_qq + jj);
	BigInteger S1(_qq, 32);
	if (S1 >= Q) return false;

	BigInteger rr = (temp2 + S1) % Q;
	BigInteger zero = BigInteger(0);
	Point G(zero, bign_curve256v1::getY());
	Point R = shamir(G, rr, q, S0);
	if (R.x == zero && R.y == zero) return false;
	byte toHash[108];
	byte bR[64];
	PI(bR, R);
	belt_hash(toHash, sizeof toHash, h_belt);
	for (size_t jj = 0; jj < 32; ++jj) if (h_belt[jj] != bR[jj]) return false;
	return true;
}
static  void bign_dh(byte *k, uint32 kSize, byte *P, byte* to){
	byte *_k = new byte[((kSize - 1 / 4) + 1) * 4];
	memset(_k, 0x00, sizeof _k);
	memcpy(_k, k, kSize);
	for (size_t jj = 0; jj < sizeof _k; jj += 4) change_endian(_k + jj);
	BigInteger K(_k, sizeof _k);
	K %= bign_curve256v1::getQ();
	Point PP;
	REV_PI(P, PP);
	Point ret = doit(PP, K);
	PI(to, ret);
}