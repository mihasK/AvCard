#ifndef __CONSTANTS_H_
#define __CONSTANTS_H_

#include "point.h"

class bign_curve256v1 : public ECurveParams {
public:
	static bign_curve256v1& const getData()  {
		static bool was_init = false;
		static bign_curve256v1 instance;
		if (!was_init) {
			//init

			was_init = true;
		}	
		return instance;
	}
private:

	bign_curve256v1(BigInteger a, BigInteger b, BigInteger p, BigInteger q, Point G) {}
	bign_curve256v1() {}
	bign_curve256v1(bign_curve256v1&) {}
	bign_curve256v1& operator=(bign_curve256v1&) {}
	~bign_curve256v1() {}

};

#endif