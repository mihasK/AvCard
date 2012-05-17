#include "SecureMessaging.h"
#include <cstring>



bool checkImito(byte* message, int size, byte* key){
	byte* imitoCmp=new byte[IMITO_LENGTH];
	belt_mac(message, size-IMITO_LENGTH, key, imitoCmp);
	int cmp=memcmp(imitoCmp, message+size-IMITO_LENGTH, IMITO_LENGTH);
	delete[IMITO_LENGTH] imitoCmp;
	return cmp==0 ? true : false;
}
void refreshSubKey(byte* keyMain, byte* key1, byte* key2){
	belt_keyrep(keyMain, 0, key1);
	belt_keyrep(keyMain, 1, key2);
}
void SecureMessaging::update(byte* shift){
	brng_hmac(keyMain, shift, keyMain,SM_KEY_SHIFT_LENGTH);
	//change keyEncr, keyMAC
	refreshSubKey(keyMain,keyEncr,keyMAC);
}
void SecureMessaging::wrap(byte* message,int length, byte* to){
	rng->get(to, SYNCHRO_LENGTH);
	belt_ctr(message, length, keyEncr,to, to+SYNCHRO_LENGTH);
	belt_mac(to, length+SYNCHRO_LENGTH, keyMAC, to+SYNCHRO_LENGTH+length);
}
void SecureMessaging::unwrap(byte* message,int length, byte* to){
	if(length< IMITO_LENGTH+SYNCHRO_LENGTH)
		throw "wrong length";
	if(!checkImito(message, length, keyMAC))
		throw "imito not match";
	belt_ctr(message+SYNCHRO_LENGTH,length-SYNCHRO_LENGTH-IMITO_LENGTH,keyEncr,message,to);
	
}
bool SecureMessaging::setTemplate(Password* pwd){
	this->pwd=pwd;
}
void SMResponser::response1(byte* request, byte* to ){ 
	 byte* paceKeyPrototype=new byte[PACE_GENPOINT_PROTOTYPE_SIZE];
	 byte* key= new byte[BELT_HASH_LENGHT];
	 belt_hash (pwd -> data,pwd->length, key);
	 belt_ecb_decr (request, PACE_REQUEST_1_SIZE, key, paceKeyPrototype); 
	 delete[BELT_HASH_LENGHT] key;
	 rng->get(paceKeyPrototype+PACE_GENPOINT_PROTOTYPE_SIZE/2, PACE_GENPOINT_PROTOTYPE_SIZE/2) ;
	 memcpy(to, paceKeyPrototype+PACE_GENPOINT_PROTOTYPE_SIZE/2, PACE_GENPOINT_PROTOTYPE_SIZE/2);
	 byte* point=new byte[BIGN_POINT_LENGHT];
	 swu(paceKeyPrototype,PACE_GENPOINT_PROTOTYPE_SIZE, point);
	 delete[PACE_KEY_EFFICIENT_LENGHT] paceKeyPrototype;

     do{ 
		 rng->get(myPACEkeyPart,PACE_KEY_EFFICIENT_LENGHT);
		 memcpy(myPACEkeyPart, (BigInteger(myPACEkeyPart,PACE_KEY_EFFICIENT_LENGHT) % bign_curve256v1::getData().q).getData(),
			 PACE_KEY_EFFICIENT_LENGHT);
	 } while(myPACEkeyPart[PACE_KEY_EFFICIENT_LENGHT-1]  ==  0);

	 bign_dh (myPACEkeyPart,PACE_KEY_EFFICIENT_LENGHT, point, to+PACE_KEY_EFFICIENT_LENGHT/2); 
	 delete[BIGN_POINT_LENGHT] point;
} 
// Va||Ta
void SMResponser::response2(byte* request, byte* to) { 
	if(!checkImito(request, PACE_REQUEST_2_SIZE,keyMAC))
		throw "imito not match";
	byte * extPoint = request;
	if  (!bign_valpubkey (extPoint))      { 
        //pwd -> dec_rc (); 
        throw "is not a valid public key"; 
     } 
	bign_dh (myPACEkeyPart, PACE_KEY_EFFICIENT_LENGHT, extPoint, keyMain); 
    refreshSubKey(keyMain,keyEncr,keyMAC);
}

void SMRequester::request1(byte* to){
	byte* prototype=new byte[PACE_GENPOINT_PROTOTYPE_SIZE/2];
	rng->get(prototype,PACE_GENPOINT_PROTOTYPE_SIZE/2);
	byte* key= new byte[BELT_HASH_LENGHT];
	belt_hash (pwd -> data,pwd->length, key);
	belt_ecb_ecnr (prototype, PACE_REQUEST_1_SIZE, key, to); 
	delete[BELT_HASH_LENGHT] key;
	delete[PACE_GENPOINT_PROTOTYPE_SIZE/2] prototype;
}
		// pubPoint||imito  ... <- prototype||pubPoint
void SMRequester::request2(byte* response, byte* to){
	byte* prototype=new byte[PACE_GENPOINT_PROTOTYPE_SIZE/2];
	rng->get(prototype,PACE_GENPOINT_PROTOTYPE_SIZE/2);
	byte* key= new byte[BELT_HASH_LENGHT];
	belt_hash (pwd -> data,pwd->length, key);
	belt_ecb_ecnr (prototype, PACE_REQUEST_1_SIZE, key, to); 
	delete[BELT_HASH_LENGHT] key;
	delete[PACE_GENPOINT_PROTOTYPE_SIZE/2] prototype;
}

SecureMessaging::SecureMessaging(void)
{
	myPACEkeyPart=new byte[PACE_KEY_EFFICIENT_LENGHT];
}


SecureMessaging::~SecureMessaging(void)
{
}
