#pragma once
#include "cardobject.h"
#include "belt.h"
#include "bign.h"
#include "rng.h"

class SecureMessaging :
	public CardObject
{
private:
	RNG* rng;
	//BELT_KEY_LENGHT
	byte* keyMain;
	byte* keyEncr;
	byte* keyMAC;
public:
	//shift ~ BELT_KEY_LENGHT(32)
	void update(byte* shift);
	void wrap(byte* message,int length, byte* to );
	void unwrap(byte* message,int length, byte* to);
	SecureMessaging(void);
	~SecureMessaging(void);
};

