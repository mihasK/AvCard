#ifndef __BELT_H
#define __BELT_H
#include "typedef.h"

#define BELT_HASH_LENGHT 256

EXTERN_C void belt_ecb_ecnr(byte *X, uint32 size, byte *Sigma, byte *to);

EXTERN_C void belt_ecb_decr(byte *X, uint32 size, byte *Sigma, byte *to);

EXTERN_C void belt_ctr(byte *X, uint32 size, byte *Sigma, byte *S, byte *to);

EXTERN_C void belt_mac(byte *X, uint32 size, byte *Sigma, byte *to);

EXTERN_C void belt_hash(byte *X, uint32 size, byte *to);

EXTERN_C void belt_hash_step(byte *X, byte *S, byte *to);

EXTERN_C void belt_keyrep(byte *X, byte b, byte *to);



#endif