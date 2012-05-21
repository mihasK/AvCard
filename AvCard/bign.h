#pragma once

#include "point.h"
#include <ctime>
#include "belt.h"
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

static void PI(byte *A, Point & b);


static void REV_PI(byte *A, Point & b);

static BOOL bign_valpubkey(byte *Q);

static void gen_rnd_data(byte *q, int len);


static void bign_sign(byte *H, byte *d, byte* to);
static  BOOL bign_verify(byte *H, byte *_q, byte *S, uint32 size);

static void bign_keywrap(byte* X, uint32 len, byte *Q, byte *token);

static BOOL bign_keyunwrap(byte *X, byte *d, byte *untoken);

static  void bign_dh(byte *k, uint32 kSize, byte *P, byte* to);

static void bign_prng(uint32 n, byte* Sigma, byte *S, byte *to);

static void brng_hmac(byte *Sigma, byte *S, byte *to, uint32 length);

