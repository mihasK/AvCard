#pragma once
#include "BigInt.h"
#include "helpers.h"
static void swu(byte *X, uint32 size, byte* P){};
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

struct bign_curve256v1 {
	static BigInteger getA() {
		BigInteger a;
		uint32 data2[] = {0x40FFFFFF ,0xFFFFFFFF ,0xFFFFFFFF ,0xFFFFFFFF ,0xFFFFFFFF ,0xFFFFFFFF ,0xFFFFFFFF ,0xFFFFFFFF};
		for (size_t jj = 0; jj < 8; ++jj) change_endian((byte*)(data2 + jj));
		memcpy(a.data, data2, sizeof data2);
		a.length = 8;
		return a;
	}
	static BigInteger getQ()  {
		BigInteger q;
		uint32 data4[] = {0x07663D26, 0x99BF5A7E, 0xFC4DFB0D, 0xD68E5CD9, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
		for (size_t jj = 0; jj < 8; ++jj) change_endian((byte*)(data4 + jj));
		memcpy(q.data, data4, sizeof data4);
		q.length = 8;
		return q;
	}
	static BigInteger getB() {
		BigInteger b;
		uint32 data3[] = {0xF1039CD6 ,0x6B7D2EB2 ,0x53928B97 ,0x6950F54C ,0xBEFBD8E4 ,0xAB3AC1D2 ,0xEDA8F315 ,0x156CCE77};
		for (size_t jj = 0; jj < 8; ++jj) change_endian((byte*)(data3 + jj));
		memcpy(b.data, data3, sizeof data3);
		b.length = 8;
		return b;
	}
	static BigInteger getP() {
		BigInteger p;
		uint32 data[] = {0x43FFFFFF ,0xFFFFFFFF ,0xFFFFFFFF ,0xFFFFFFFF ,0xFFFFFFFF ,0xFFFFFFFF ,0xFFFFFFFF ,0xFFFFFFFF};
		for (size_t jj = 0; jj < 8; ++jj) change_endian((byte*)(data + jj));
		memcpy(p.data, data, sizeof data);
		p.length = 8;
		return p;
	}
	static BigInteger getY() {
		BigInteger yG;
		uint32 data5[] = {0x936A5104 ,0x18CF291E ,0x52F608C4 ,0x66399178 ,0x5D83D651 ,0xA3C9E45C ,0x9FD616FB ,0x3CFCF76B};
		for (size_t jj = 0; jj < 8; ++jj) change_endian((byte*)(data5 + jj));
		memcpy(yG.data, data5, sizeof data5);
		yG.length = 8;
		return yG;
	}
};

struct PointProj {
	BigInteger x, y, z;
	PointProj(const BigInteger &x, const BigInteger &y, const BigInteger &z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	friend bool operator ==(const PointProj &_1, const PointProj &_2) {
		return _1.x == _2.x && _1.y == _2.y && _1.z == _2.z;
	}

	friend PointProj operator +(PointProj _1, PointProj _2) {
		BigInteger P = bign_curve256v1::getP();
		if (_1 == _2) {
			BigInteger _3(3);
			BigInteger A = ((_1.x << 2) + powMod(_1.y, 2, P)) % P;
			BigInteger B = ((powMod(_1.y, 4, P)) << 3)%P;
			BigInteger Z_2 = powMod(_1.z, 2, P);
			BigInteger C =   (((_3 * (_1.x + P - Z_2))%P) * ((_1.x + Z_2)%P))%P;
			BigInteger D = ((powMod(C, 2, P) + P - ((A << 1) % P))) % P;
			BigInteger X = D;
			BigInteger Y = (((C * (A + P - D))% P) + P - B)%P;
			BigInteger Z = (((_1.x * _1.y) % P) << 1) % P;
			return PointProj(X, Y, Z);		
		} else {
			BigInteger A = (_2.x * powMod(_1.z, 2, P)) % P;
			BigInteger B = (_2.y * powMod(_1.z, 3, P)) % P;
			BigInteger C = (A + P - _1.x) % P;
			BigInteger D = (B + P - _1.y) % P;
			BigInteger X = (powMod(D, 2, P)   + P - ((powMod(C, 2, P) * (C + (_1.x << 1))) % P) ) % P;
			BigInteger Y = (( D * ((((_1.x * powMod(C, 2, P)) % P  + P - X)) % P) ) % P + P - ((_1.y * powMod(C, 3, P)) % P)) % P;
			BigInteger Z = (_1.z * C) % P;
			return PointProj(X, Y, Z);
		}
	}



	void toAffine() {
		BigInteger zero(0), one(1);
		if (this->z == 0) {
			this->x = zero;
			this->y = zero;
			return;
		}
		BigInteger P = bign_curve256v1::getP();
		BigInteger Z = powMod(this->z,  P - 2, P);
		Z = powMod(Z, 3, P);
		this->x = (this->x * Z ) % P;
		this->x = (this->x * this->z) % P;
		this->y = (this->y * Z) % P;
		this->z = one;
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
		if (this->x == 0 && this->y == 0) {
			this->x = another.x;
			this->y = another.y;
			return *this;
		}

		if (another.x == 0 && another.y == 0) return *this;
		PointProj ret = PointProj(this->x, this->y, BigInteger(1)) + PointProj(another.x, another.y, BigInteger(1));
		ret.toAffine();
		this->x = ret.x;
		this->y = ret.y;
		return *this;
	} 
	friend Point doit(Point a, BigInteger k) {
		Point ret(a.x, a.y);
		BigInteger zero(0);
		uint32 sz = k.getBitsLength() - 1;
		for (; sz >=0; --sz) {
			if (k.getBit(sz)) {
					ret += ret;
					ret += a;
			} else {
				ret += ret;
			}
		}
		return ret;
	}
	friend Point shamir(Point a, BigInteger k, Point b, BigInteger l) {
		Point C = a;
		C += b;
		Point R(BigInteger(0), BigInteger(0));
		uint32 m = max(k.getBitsLength(), l.getLength());
		while (m >=0 ) {
			uint32 t = ((int)k.getBit(m) << 1) | l.getBit(m);
			R += R;
			if (t == 3) R += C;
			else if (t == 2) R += a;
			else if (t == 1) R += b;
		}
		return R;
	}
};





