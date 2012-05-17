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
	belt_keyrep(keyMain, 1, newKeyMAC);
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
bool SecureMessaging::setTemplate(Password* pwd){
	this->pwd=pwd;
}
void SecureMessaging::response1(byte* request, byte* to ){ 
	 byte* paceKeyPrototype=new byte[SM_PACE_KEY_PROTOTYPE_SIZE];
	 byte* key= new byte[BELT_HASH_LENGHT];
	 belt_hash (pwd -> data,pwd->lenght, key);
	 belt_ecb_decr (request, SM_PACE_KEY_PROTOTYPE_SIZE/2, key, paceKeyPrototype); 
	 delete[BELT_HASH_LENGHT] key;
	 rng->get(paceKeyPrototype+SM_PACE_KEY_PROTOTYPE_SIZE/2, SM_PACE_KEY_PROTOTYPE_SIZE/2) ;
	 memcpy(to, paceKeyPrototype+SM_PACE_KEY_PROTOTYPE_SIZE/2, SM_PACE_KEY_PROTOTYPE_SIZE/2);
	 byte* point=new byte[BIGN_POINT_LENGHT];
	 swu(paceKeyPrototype,SM_PACE_KEY_PROTOTYPE_SIZE, point);
	 delete[SM_PACE_KEYPART_LENGHT] paceKeyPrototype;

     do{ 
		 rng->get(myPACEkeyPart,SM_PACE_KEYPART_LENGHT);
		 memcpy(myPACEkeyPart, (BigInteger(myPACEkeyPart,SM_PACE_KEYPART_LENGHT) % get_q()).getData(),
			 SM_PACE_KEYPART_LENGHT);
	 } while(myPACEkeyPart[SM_PACE_KEYPART_LENGHT-1]  ==  0);

	 bign_dh (myPACEkeyPart,SM_PACE_KEYPART_LENGHT, point, BIGN_POINT_LENGHT,  to+SM_PACE_KEY_PROTOTYPE_SIZE/2); 
	 delete[BIGN_POINT_LENGHT] point;
} 
/*void SecureMessaging::response2(byte* request, byte* to) { 
     byte [64]    va  =  req2 [0..63]; 
      if  (! bign_valpubkey (va))      { 
        pwd -> dec_rc (); 
        return    null ; 
     } 
     keyMain    =  bign_dh (ub ,   va) [0..31]; 
     keyEncr    =  belt_keyrep ( keyMain ,     0); 
     keyMAC    =  belt_keyrep ( keyMain ,     1); 
     byte [8]    ta  = req2 [64..71]; 
      if  (ta  !=  belt_mac (vb ,    keyMAC ))   { 
        pwd -> dec_rc (); 
        deactivate (); 
        return    null ; 
     } */
SecureMessaging::SecureMessaging(void)
{
	myPACEkeyPart=new byte[SM_PACE_KEYPART_LENGHT];
}


SecureMessaging::~SecureMessaging(void)
{
}
