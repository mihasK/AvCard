#include "SecureMessaging.h"
#include <cstring>

#define SYNCHRO_LENGTH BELT_SYNCHRO_LENGHT
#define IMITO_LENGTH BELT_MAC_LENGHT

void SecureMessaging::update(byte* shift){
	byte* newKey=new byte[BELT_KEY_LENGHT];
	brng_hmac(keyMain, shift, newKey,(int)BELT_KEY_LENGHT);
	delete[BELT_KEY_LENGHT] keyMain;
	keyMain=newKey;

	delete[BELT_KEY_LENGHT] keyEncr;
	delete[BELT_KEY_LENGHT] keyMAC;

	//change keyEncr
	byte* newKeyEncr=new byte[BELT_KEY_LENGHT];
	belt_keyrep(keyMain, 0, newKeyEncr);
	keyEncr=newKeyEncr;

	//change keyMAC
	byte* newKeyMAC=new byte[BELT_KEY_LENGHT];
	belt_keyrep(keyMain, 0, newKeyMAC);
	keyMAC=newKeyMAC;
}
void SecureMessaging::wrap(byte* message,int length, byte* to){
	rng->get(to, SYNCHRO_LENGTH);
	belt_ctr(message, length, keyEncr,to, to+SYNCHRO_LENGTH);
	belt_mac(to, length+SYNCHRO_LENGTH, keyMAC, to+SYNCHRO_LENGTH+length);
}
void SecureMessaging::unwrap(byte* message,int length, byte* to){
	if(length< IMITO_LENGTH+SYNCHRO_LENGTH)
		throw "wrong length";

	byte* imitoCmp=new byte[IMITO_LENGTH];
	belt_mac(message, length+SYNCHRO_LENGTH, keyMAC, imitoCmp);
	delete[IMITO_LENGTH] imitoCmp;

	if(memcmp(imitoCmp, message+length-IMITO_LENGTH, IMITO_LENGTH) != NULL)
		throw "imito not match";
	belt_ctr(message+SYNCHRO_LENGTH,length-SYNCHRO_LENGTH-IMITO_LENGTH,keyEncr,message,to);
	
}
SecureMessaging::SecureMessaging(void)
{
}


SecureMessaging::~SecureMessaging(void)
{
}
