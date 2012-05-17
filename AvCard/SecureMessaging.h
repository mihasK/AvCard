#pragma once
#include "cardobject.h"
#include "belt.h"
#include "bign.h"
#include "rng.h"
#include "password.h"
#include "helpers.h"
#include "constants.h"

#define SM_PACE_KEY_PROTOTYPE_SIZE 32
#define SM_PACE_KEYPART_LENGHT 32

class SecureMessaging :
	public CardObject
{
private:
	Password* pwd;
	RNG* rng;
	//BELT_KEY_LENGHT
	byte* keyMain;
	byte* keyEncr;
	byte* keyMAC;
	//32 byte
	byte* myPACEkeyPart;
public:
	//[ cmd :< MSE : Set  AT >] 
	bool setTemplate(Password* pwd);
	//[ cmd :< MSE : General    Authenticate >]
    void response1 (byte* request, byte* to );
	//[ cmd :< MSE : General    Authenticate >] 
	void response2 (byte* request, byte* to);
	//shift ~ BELT_KEY_LENGHT(32)
	void update(byte* shift);
	void wrap(byte* message,int length, byte* to );
	void unwrap(byte* message,int length, byte* to);
	SecureMessaging(void);
	~SecureMessaging(void);
};

