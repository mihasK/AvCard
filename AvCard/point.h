#pragma once
#include "BigInt.h"

typedef enum {
	NULL_POINT = 0,
	AFFINE_POINT = 1,
	PROJECTIVE_POINT = 2
} PointType;
struct ECurveParams {
	BigInteger a, b, p, q;
	BigInteger yG;
	uint64 seed;
	ECurveParams() {}
	ECurveParams(BigInteger a, BigInteger b, BigInteger p, BigInteger q, BigInteger  yG) {
		this->a = a;
		this->b = b;
		this->q = q;
		this->p = p;
		this->yG = yG;
	}
};

class bign_curve256v1 : public ECurveParams {

public:
	static bign_curve256v1  getData()   {
		static bool was_init = false;
		static bign_curve256v1 instance;
		if (!was_init) {
			//init
			was_init = true;
		}	
		return instance;
		
	}
private:

	bign_curve256v1(BigInteger a, BigInteger b, BigInteger p, BigInteger q, BigInteger G) {}
	bign_curve256v1() {}
	bign_curve256v1(bign_curve256v1&) {}
	bign_curve256v1& operator=(bign_curve256v1&) {}
};

struct PointProj {
	BigInteger x, y, z;
	PointProj(const BigInteger &x, const BigInteger &y, const BigInteger &z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	PointProj& operator += (const PointProj& another) {
		if (another.x == this->x && another.y == this->y && another.z == this->z) {
			//TODO
		} else {
		BigInteger P = bign_curve256v1::getData().p;

		BigInteger Z1_2 = (this->z * this->z) % P;
				BigInteger A = (another.x * Z1_2) %P;
		BigInteger B = (another.y * this->z) % P;
		(B *= Z1_2) %= P;
		BigInteger C = (A + P - this->x) % P;
		BigInteger D = (B + P - this->y) % P;
		BigInteger C_2 = (C * C) % P;
		BigInteger X = this->x;
		this->x = (D * D % P + P - (C_2 * C % P + (this->x << 1U) * C_2 % P) % P)%P;
		this->y = ((D * ((X * C_2 % P + P -this->x) % P) % P) + P - this->y * C_2 % P * C % P) % P;
		this->z *= C;
		}
		return *this;
	}

	void toAffine() {
		//TODO
	}
};


class Point {
private:
	BigInteger x, y;
	PointType type;
public: 
	Point() {
		this->type = NULL_POINT;		
	}
	Point(const BigInteger &x, const BigInteger &y, const BigInteger &z) {
		this->type = PROJECTIVE_POINT;
		this->x = x;
		this->y = y;
	}

	Point& operator+=(Point another) {
		PointProj A(this->x, this->y, 1);
		PointProj B(another.x, another.y, 1);
		A += B;
		A.toAffine();
		if (A.z == 0) {
			this->type = NULL_POINT;
		} else {
			this->x = A.x;
			this->y = A.y;
		}
		return *this;
	} 
	
};





