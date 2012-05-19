#pragma once

#include "point.h"

#define BIGN_POINT_LENGHT 64
#define BIGN_PRIVKEY_LENGHT 32
#define BIGN_PUBKEY_LENGHT BIGN_POINT_LENGHT

typedef struct _bign_privkey {
	BigInteger d;
} bign_privkey;

typedef struct _bign_pubkey {
	Point Q;
} bign_pubkey;

 BOOL bign_valpubkey(byte *Q){return true;}

 void bign_sign(byte *H, byte *d, byte* to){}

 BOOL bign_verify(byte *H, byte *Q, byte *S, uint32 size){return true;}

 void bign_keywrap(byte* X, byte *Q, byte *token){}

 void bign_keyunwrap(byte *X, byte *d, byte *untoken){}

 void bign_dh(byte *k, uint32 kSize, byte *P, byte* to){}

 void bign_prng(uint32 n, byte* Sigma, byte *S, byte *to){}

 void brng_hmac(byte *Sigma, byte *S, byte *to, uint32 length){}

