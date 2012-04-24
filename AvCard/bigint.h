#ifndef __BIGINT_H_
#define __BIGINT_H_
#include "typedef.h"

#define MAX_LEN 16

typedef struct {
	uint32 length;
	uint32 data[MAX_LEN + 1];
} BigInt, * pBigInt;

typedef enum {
	OVERFLOW_ERROR = 1,
	DIVIDE_BY_ZERO_ERROR = 2, 
	SUCCESS_OPERATION = 0
} ERROR_CODE;


typedef enum {
	LESS = -1, 
	EQUAL = 0,
	GREATER = 1
} COMPARE_RESULT;

EXTERN_C void vRead_BigInt(pBigInt to, void * from, uint32 size);

EXTERN_C COMPARE_RESULT eCompare_BigInt(pBigInt a, pBigInt b);

EXTERN_C ERROR_CODE eAdd_BigInt(pBigInt to, pBigInt a, pBigInt b);

EXTERN_C ERROR_CODE eSubtract_BigInt(pBigInt to, pBigInt a, pBigInt b);

EXTERN_C ERROR_CODE eMultiply_BigInt(pBigInt to, pBigInt a, pBigInt b);

EXTERN_C ERROR_CODE eDivide_BigInt(pBigInt div, pBigInt rem, pBigInt a, pBigInt b);

EXTERN_C ERROR_CODE eAdd_BigInt_Mod(pBigInt to, pBigInt a, pBigInt b, pBigInt mod);

EXTERN_C ERROR_CODE eSubtract_BigInt_Mod(pBigInt to, pBigInt a, pBigInt b, pBigInt mod);

EXTERN_C ERROR_CODE eMultiply_BigInt_Mod(pBigInt to, pBigInt a, pBigInt b, pBigInt mod);

EXTERN_C ERROR_CODE eDivide_BigInt_Mod(pBigInt div, pBigInt rem, pBigInt a, pBigInt b, pBigInt mod);

EXTERN_C ERROR_CODE eExp_BigInt(pBigInt to, pBigInt a, pBigInt e);

EXTERN_C ERROR_CODE eExp_BigInt_Mod(pBigInt to, pBigInt a, pBigInt e, pBigInt mod);

EXTERN_C ERROR_CODE eSquareRoot_BigInt_Mod(pBigInt to, pBigInt a, pBigInt mod);

EXTERN_C BOOL bIsProbablePrime_BigInt(pBigInt a);

#endif
