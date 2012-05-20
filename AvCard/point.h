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
			BigInteger P = bign_curve256v1::getData().p;
			this->z = ((another.x * another.x) % P * (another.z * another.z) % P)%P;
		} else {
			/*A = Y2. Z12 + Y1
			B = X2. Z1 + X1
			C = Z1. B
			D = B2.(C + a. Z12)
			Z3 = C2
			E = A.C
			X3 = A2 + D + E
			F = X3 + X2. Z3
			G = X3 + Y2. Z3
			Y3 = E. F + Z3.G
			Z2 = 1, */
			BigInteger P = bign_curve256v1::getData().p;

			BigInteger Z1_2 = (this->z * this->z) % P;
			BigInteger A = ((another.y * Z1_2) % P + this->y) % P;
			BigInteger B = ((another.x * this->z) % P + this->x) % P;
			BigInteger C = (this->x * B) % P;
			BigInteger D = (B * B) % P;
			BigInteger t = (C + (bign_curve256v1::getData().a * Z1_2) % P) % P;
			D *= t %= P;
			this->z = (C * C) % P;
			BigInteger E = (A * C) % P;
			this->x = ((A * A) % P + D + E) % P;
			BigInteger F = (this->x + another.x * this->z) % P;
			BigInteger G = (this->x + another.y * this->z) % P;
			this->y = (E * F % P + this->z * G) % P;
		}
		return *this;
	}

	void toAffine() {
		BigInteger Z = this->z;
		BigInteger P = bign_curve256v1::getData().p;
		Z = powMod(Z, P - 2, P);
		BigInteger Z3 = (Z * Z ) % P;
		(this->x *= Z3) %= P;
		(this->y *= Z3) %= P;
		this->x *= this->z;
		(this->x)%=P;
	}
};


class Point {
public:
	BigInteger x, y;
	PointType type;
public: 
	Point() {
		this->type = NULL_POINT;		
	}
	Point(const BigInteger &x, const BigInteger &y) {
		this->type = AFFINE_POINT;
		this->x = x;
		this->y = y;
	}

	Point& operator+=(Point another) {
		/*PointProj A(this->x, this->y, 1);
		PointProj B(another.x, another.y, 1);
		A += B;
		A.toAffine();
		if (A.z == 0) {
		this->type = NULL_POINT;
		} else {
		this->x = A.x;
		this->y = A.y;
		}
		return *this;*/
		if (this->x == 0 && this->y == 0) {
			this->x = another.x;
			this->y = another.y;
			return *this;
		}

		if (another.x == 0 && another.y == 0) return *this;
		BigInteger lambda;
		BigInteger P = bign_curve256v1::getData().p;
		if (another.x == this->x && another.y == this->y) {
			lambda = (BigInteger(3)  * this->x * this->x) % P;
			(lambda += bign_curve256v1::getData().a) %= P;
			BigInteger promo = (this->y << 1) % P;
			promo %= P;
			BigInteger tt = powMod(promo, P  - BigInteger(2), P);
			(lambda *= tt) %= P;
		} else {
			lambda = (another.y  + P - this->y) % P;
			BigInteger promo = (another.x + P - this->x) % P;
			BigInteger tt = powMod(promo, P - BigInteger(2), P);
			(lambda *= tt) %= P;
		}
		BigInteger X = (lambda * lambda % P   + P - this->x + P - another.x) % P;
		BigInteger Y = (lambda * (this->x + P - X) % P + P - this->y) % P;
		this->x = X;
		this->y = Y;
		return *this;
	} 
	friend Point doit(Point a, BigInteger k) {
		Point ret(BigInteger(0), BigInteger(0));
		BigInteger zero(0);
		while (k > zero) {
			if (k.data[0] & 1) {
				ret += a;
				k-=BigInteger(1);
			} else {
				a += a;
				k >>= 1;
			}
		}
		return ret;
	}
	friend Point shamir(Point a, BigInteger k, Point b, BigInteger l) {
		Point ret1 = doit(a, k), ret2 = doit(b, l);
		ret1+=ret2;
		return ret1;
	}
};





