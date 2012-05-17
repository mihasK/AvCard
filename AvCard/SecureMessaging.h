#pragma once
#include "cardobject.h"
#include "belt.h"
#include "bign.h"
#include "rng.h"
#include "password.h"
#include "helpers.h"
#include "constants.h"

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
	void refreshSubKey();
	void rndNumberUnderQ(byte* to);
public:
	//[ cmd :< MSE : Set  AT >] 
	bool setTemplate(Password* pwd);
	
	//shift ~ BELT_KEY_LENGHT(32)
	void update(byte* shift);
	void wrap(byte* message,int length, byte* to );
	void unwrap(byte* message,int length, byte* to);

	void deactivate(){
		CardObject::deactivate();
		delete[BIGN_POINT_LENGHT] keyMain;
		delete[BIGN_POINT_LENGHT] keyEncr;
		delete[BIGN_POINT_LENGHT] keyMAC;
		delete pwd;
	}
};

class SMResponser:public SecureMessaging{
private:
	//32 byte
	byte* myPACEkeyPart;
public:
	//[ cmd :< MSE : General    Authenticate >]
    void response1 (byte* request1, byte* to );
	//[ cmd :< MSE : General    Authenticate >] 
	void response2 (byte* request2, byte* to);
	SMResponser(void){
		myPACEkeyPart=new byte[PACE_KEY_EFFICIENT_LENGHT];
	}
	~SMResponser(void){
		delete[PACE_KEY_EFFICIENT_LENGHT] myPACEkeyPart;
	}
}

class SMRequester:public SecureMessaging{
private:
	byte* fullPointProto;
	//for check
	byte* myPoint;
public:
	void request1 (byte* to );
	//[ cmd :< MSE : General    Authenticate >]
    void request2 (byte* response1, byte* to );

	void check(byte* response2);
	
	SMRequester(void){
		fullPointProto=new byte[PACE_GENPOINT_PROTOTYPE_SIZE];
		myPoint=new byte[BIGN_POINT_LENGHT];
	}
	~SMRequester(void){
		delete[PACE_GENPOINT_PROTOTYPE_SIZE] fullPointProto;
		delete[BIGN_POINT_LENGHT] myPoint;
	}
}

