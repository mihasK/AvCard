#ifndef __POINT_H_
#define __POINT_H_

#include "BigInt.h"




typedef enum {
	NULL_POINT = 0,
	AFFINE_POINT = 1,
	PROJECTIVE_POINT = 2
} PointType;



class Point {
private:
	BigInteger x, y, z;
	PointType type;
public: 
	Point() {
		this->type = NULL_POINT;		
	}
	Point(const BigInteger &x, const BigInteger &y, const BigInteger &z) {
		this->type = PROJECTIVE_POINT;
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Point(const BigInteger &x, const BigInteger &y) {
		this->type = AFFINE_POINT;
		this->x = x;
		this->y = y;
	}
	
};

struct ECurveParams {

	BigInteger a, b, p, q;
	Point G;
	ECurveParams() {}
	ECurveParams(BigInteger a, BigInteger b, BigInteger p, BigInteger q, Point G) {
		this->a = a;
		this->b = b;
		this->q = q;
		this->p = p;
		this->G = G;
	}
};


#endif