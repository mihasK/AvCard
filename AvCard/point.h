#ifndef __POINT_H_
#define __POINT_H_

#include "BigInt.h"

typedef enum {
	NULL_POINT = 0,
	AFFINE_POINT = 1,
	PROJECTIVE_POINT = 2
} POINT_EC_TYPE;


typedef struct {
	POINT_EC_TYPE   type;    
	BigInteger	x;       
	BigInteger	y;      
} PointECAf, *pPointECAf;

typedef struct {
	BigInteger    p;   
	BigInteger    a;   
	BigInteger    b;   
	BigInteger q;   
	PointECAf  G; 
} ParamEC, *pParamEC;


typedef struct {	
	  POINT_EC_TYPE   type;  
	BigInteger	x;         
	BigInteger	y;         
	BigInteger	z;         
} PointECPr, *pPointECPr;


EXTERN_C BOOL EC_IsPointAf(pPointECAf P, pParamEC   param);

EXTERN_C void EC_PointPrToAf(pPointECAf R, pPointECPr P, pParamEC ec);


EXTERN_C void EC_DoublePointPr(pPointECPr P, pParamEC param);

EXTERN_C void EC_AddPointPrAf(pPointECPr P,pPointECAf Q, pParamEC param);

EXTERN_C void EC_AddPointAf(pPointECAf R, pPointECAf Q1, pPointECAf Q2, pParamEC   param );

EXTERN_C void EC_MultPointAf(pPointECAf P, BigInteger  K, pParamEC   param);

EXTERN_C void EC_AddMultPointAf(pPointECAf R, pPointECAf P1, BigInteger K1, pPointECAf P2, BigInteger K2, pParamEC param);

EXTERN_C BOOL EC_PointAf_koordY(pPointECAf P, pParamEC param);

#endif

