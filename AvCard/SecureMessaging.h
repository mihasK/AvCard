#pragma once
#include "cardobject.h"
#include "belt.h"
#include "bign.h"
#include "rng.h"
#include "password.h"
#include "helpers.h"

#define SM_KEY_SHIFT_LENGTH			BELT_KEY_LENGHT
#define SYNCHRO_LENGTH				BELT_SYNCHRO_LENGHT
#define IMITO_LENGTH				BELT_MAC_LENGHT
#define PACE_GENPOINT_PROTOTYPE_SIZE	32
#define PACE_KEY_EFFICIENT_LENGHT		32
#define PACE_REQUEST_1_SIZE		PACE_GENPOINT_PROTOTYPE_SIZE/2
#define PACE_REQUEST_2_SIZE		BIGN_POINT_LENGHT+IMITO_LENGTH


class SecureMessaging :
	public CardObject
{
protected:
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
	
	//shift ~ BELT_KEY_LENGHT(32)
	void update(byte* shift);
	void wrap(byte* message,int length, byte* to );
	void unwrap(byte* message,int length, byte* to);
	SecureMessaging(void);
	~SecureMessaging(void);
};

class SMResponser:public SecureMessaging{
	//[ cmd :< MSE : General    Authenticate >]
    void response1 (byte* request1, byte* to );
	//[ cmd :< MSE : General    Authenticate >] 
	void response2 (byte* request2, byte* to);
}

class SMRequester:public SecureMessaging{
	void request1 (byte* to );
	//[ cmd :< MSE : General    Authenticate >]
    void request2 (byte* response1, byte* to );
	//[ cmd :< MSE : General    Authenticate >] 
	void request3 (byte* response2, byte* to);
}

