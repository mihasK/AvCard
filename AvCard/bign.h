#pragma once

#include "point.h"
#include <ctime>
#define BIGN_POINT_LENGHT 64
#define BIGN_PRIVKEY_LENGHT 32
#define BIGN_PUBKEY_LENGHT BIGN_POINT_LENGHT
static byte OID[] = { 0x06, 0x06, 0x09, 0x2A, 0x70, 0x00, 0x02, 0x00, 0x22, 0x65, 0x1F, 0x51 };
typedef struct _bign_privkey {
	BigInteger d;
} bign_privkey;

typedef struct _bign_pubkey {
	Point Q;
} bign_pubkey;

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
	 BigInteger _P = bign_curve256v1::getData().p;
	 REV_PI(Q, P);
	 if (P.x >= _P || P.y >= _P) return false;
	 BigInteger left = (P.y * P.y) % _P;
	 BigInteger right = ((P.x * P.x % _P) * P.x % _P + (bign_curve256v1::getData().a * P.x) % _P + bign_curve256v1::getData().b ) % _P;
	 return (left == right);
 }

 static void gen_rnd_data(byte *q, int len) {
	 srand(time(NULL));
	for (size_t i=  0; i < len; ++i) q[i] = rand();
 }


 static void bign_sign(byte *H, byte *d, byte* to) {
	byte rand[32];
	gen_rnd_data(rand, 32);
	BigInteger k(rand, 32);
	BigInteger P = bign_curve256v1::getData().p;
	k %= P;
	byte dd[32];
	memcpy(dd, d, 32);
	for (size_t jj = 0; jj < 32; jj += 4) change_endian(dd + jj);
	BigInteger D(dd, 32);
	BigInteger Q = bign_curve256v1::getData().q;
	Point G(BigInteger(0), bign_curve256v1::getData().yG);
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
static  BOOL bign_verify(byte *H, byte *_q, byte *S, uint32 size){
	Point q;
	REV_PI(_q, q);
	BigInteger Q = bign_curve256v1::getData().q;
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
	Point G(zero, bign_curve256v1::getData().yG);
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

static void bign_keywrap(byte* X, byte *Q, byte *token){}

static void bign_keyunwrap(byte *X, byte *d, byte *untoken){}

static  void bign_dh(byte *k, uint32 kSize, byte *P, byte* to){
	 byte *_k = new byte[((kSize - 1 / 4) + 1) * 4];
	 memset(_k, 0x00, sizeof _k);
	 memcpy(_k, k, kSize);
	 for (size_t jj = 0; jj < sizeof _k; jj += 4) change_endian(_k + jj);
	 BigInteger K(_k, sizeof _k);
	 K %= bign_curve256v1::getData().q;
	 Point PP;
	 REV_PI(P, PP);
	 Point ret = doit(PP, K);
	 PI(to, ret);
 }

 static void bign_prng(uint32 n, byte* Sigma, byte *S, byte *to){}

static void brng_hmac(byte *Sigma, byte *S, byte *to, uint32 length){}

