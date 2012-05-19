#include "SecureMessaging.h"
#include <cstring>


bool checkImito(byte* message, int size, byte* imito, byte* key){
	byte* imitoCmp=new byte[IMITO_LENGTH];
	belt_mac(message, size, key, imitoCmp);
	int cmp=memcmp(imitoCmp, imito, IMITO_LENGTH);
	delete[IMITO_LENGTH] imitoCmp;
	return cmp==0 ? true : false;
}

bool checkImito(byte* messageWithImito, int size, byte* key){
	if(size<=IMITO_LENGTH)
		throw "wrong message";
	return checkImito(messageWithImito,size-IMITO_LENGTH, 
		messageWithImito+size-IMITO_LENGTH,key);
}

void SecureMessaging:: refreshSubKey(){
	belt_keyrep(keyMain, 0, keyEncr);
	belt_keyrep(keyMain, 1, keyMAC);
}

void SecureMessaging::update(byte* shift){
	brng_hmac(keyMain, shift, keyMain,SM_KEY_SHIFT_LENGTH);
	//change keyEncr, keyMAC
	refreshSubKey();
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
	return true;
}
// encr(prototype) <- ...
void SMRequester::request1(){
	byte* to = new byte[PACE_REQUEST_1_SIZE];
	byte* fullPointProto=new byte[PACE_GENPOINT_PROTOTYPE_SIZE];
	rng->get(fullPointProto,PACE_GENPOINT_PROTOTYPE_SIZE/2);
	byte* key= new byte[BELT_HASH_LENGHT];
	belt_hash (pwd -> data,pwd->length, key);
	//encr(prototype) >>
	belt_ecb_encr (fullPointProto, PACE_REQUEST_1_SIZE, key, to); 
	send(to, PACE_REQUEST_1_SIZE);

	belt_ecb_encr (fullPointProto, PACE_REQUEST_1_SIZE, key, to); 
	delete[BELT_HASH_LENGHT] key;
	delete[PACE_REQUEST_1_SIZE] to;
}

//	prototype||pubPoint <- 	...	<-	encr(prototype)
void SMResponser::response1(byte* request){ 
	 byte* to = new byte[PACE_RESPONSE_1_SIZE];
	 byte* fullPointProto=new byte[PACE_GENPOINT_PROTOTYPE_SIZE];
	 byte* key= new byte[BELT_HASH_LENGHT];
	 belt_hash (pwd -> data,pwd->length, key);
	 belt_ecb_decr (request, PACE_REQUEST_1_SIZE, key, fullPointProto); 
	 
	 rng->get(fullPointProto+PACE_GENPOINT_PROTOTYPE_SIZE/2, PACE_GENPOINT_PROTOTYPE_SIZE/2) ;
	 
	 //prototype
	 memcpy(to, fullPointProto+PACE_GENPOINT_PROTOTYPE_SIZE/2, PACE_GENPOINT_PROTOTYPE_SIZE/2);
	 
	 byte* point=new byte[BIGN_POINT_LENGHT];
	 swu(fullPointProto,PACE_GENPOINT_PROTOTYPE_SIZE, point);

	 rndNumberUnderQ(myPACEkeyPart);

	 //pubPoint>>
	 bign_dh (myPACEkeyPart,PACE_KEY_EFFICIENT_LENGHT, point, to+PACE_KEY_EFFICIENT_LENGHT/2); 
	 send(to, PACE_RESPONSE_1_SIZE);

	 delete[PACE_RESPONSE_1_SIZE] to;
	 delete[BELT_HASH_LENGHT] key;
	 delete[PACE_GENPOINT_PROTOTYPE_SIZE] fullPointProto;
	 delete[BIGN_POINT_LENGHT] point;
}

	// pubPoint||imito  ... <- prototype||pubPoint
void SMRequester::request2(byte* response){
	byte* to = new byte[PACE_REQUEST_2_SIZE];
	byte * extPoint = response+PACE_GENPOINT_PROTOTYPE_SIZE/2;
	if  (!bign_valpubkey (extPoint))      { 
        //pwd -> dec_rc (); 
        throw "is not a valid public key"; 
    } 
	memcpy(fullPointProto+PACE_GENPOINT_PROTOTYPE_SIZE/2,response, 
		PACE_GENPOINT_PROTOTYPE_SIZE/2);
	byte* point=new byte[BIGN_POINT_LENGHT];
	swu(fullPointProto,PACE_GENPOINT_PROTOTYPE_SIZE, point);
	byte* myPointEfficient=new byte[PACE_KEY_EFFICIENT_LENGHT];
	rndNumberUnderQ(myPointEfficient);

	//pubPoint>>
	bign_dh (myPointEfficient,PACE_KEY_EFFICIENT_LENGHT, point, to); 
	
	//imito>>
	belt_mac(to, BIGN_POINT_LENGHT, keyMAC, to+BIGN_POINT_LENGHT);
	send(to, PACE_REQUEST_2_SIZE);

	//common point is a new keyMain
	bign_dh (myPointEfficient, PACE_KEY_EFFICIENT_LENGHT, extPoint, keyMain);
	refreshSubKey();

	delete[BIGN_POINT_LENGHT] point;
	delete[PACE_KEY_EFFICIENT_LENGHT] fullPointProto;
	delete[PACE_REQUEST_2_SIZE] to;
}

//	imito	<-		...  <-  pubPoint||imito
void SMResponser::response2(byte* request) { 
	byte* to = new byte[PACE_RESPONSE_2_SIZE];
	if(!checkImito(request, PACE_REQUEST_2_SIZE,keyMAC))
		throw "imito not match";
	byte * extPoint = request;
	if  (!bign_valpubkey (extPoint))      { 
        //pwd -> dec_rc (); 
		throw "is not a valid public key"; 
    }

	//imito>>
	belt_mac(extPoint, BIGN_POINT_LENGHT, keyMAC, to);
	

	//common point
	bign_dh (myPACEkeyPart, PACE_KEY_EFFICIENT_LENGHT, extPoint, keyMain); 
    refreshSubKey();

	send(to, PACE_RESPONSE_2_SIZE);

	activate();
	delete[PACE_RESPONSE_2_SIZE] to;
}
//				..?	<-	imito
void SMRequester::check(byte* response){
	byte* imito=response;
	if(!checkImito(myPoint, BIGN_POINT_LENGHT, imito,keyMAC)){
		throw "imito not match";
	}
	activate();
}

void SecureMessaging::rndNumberUnderQ(byte* to){
	byte* proto=new byte[PACE_KEY_EFFICIENT_LENGHT];
	do{ 
		 rng->get(to,PACE_KEY_EFFICIENT_LENGHT);
		 memcpy(to, (BigInteger(proto,PACE_KEY_EFFICIENT_LENGHT) % bign_curve256v1::getData().q).getData(),
			 PACE_KEY_EFFICIENT_LENGHT);
	 } while(to[PACE_KEY_EFFICIENT_LENGHT-1]  ==  0);
	delete[PACE_KEY_EFFICIENT_LENGHT] proto;

}
