#pragma once
#include <iostream>
#include <utility>
#include "cardobject.h"
#include <string.h>
#include <cstring>
#include "belt.h"
#include "bign.h"
using namespace std;

#define PUBKEY_LENGHT BIGN_PUBKEY_LENGHT

class Cert :
	public CardObject
{
	const static Cert trustedCert;
private:
	string subject;
	//byte[PUBKEY_LENGHT]
	byte* pubkey;
	//byte[BELT_HASH_LENGHT]
	byte* signature;
	Cert* issuer;
		
public:
	

	byte* getSignature() const{
		return signature;
	}
	void setSignature(byte* signature){
		this->signature=signature;
	}
	
	byte* getPubkey() const {
		return pubkey;
		
	}
	//byte[64]
	byte* pubkey_ri() const  {
	}
	Cert* getIssuer() const  {
		return issuer;
	}
	string getSubject() const  {
		return subject;
	}
	byte* name() const  {
	//return (subject() + серийный номер);
	}
	byte* credentials() const  {
	}
	bool authority() const  {
	}
	//byte[8]
	byte* not_before() const  {
	}
	//byte[8]
	byte* not_after() const  {
	}
	bool verify(const Cert& cert)  {
		if (subject.compare(cert.getIssuer()->getSubject())!=0)
			return false;
		//if (Timer::date() > cert.date())
			//return false;
		string tbs= cert.get_tbsCertificate();
		byte* sig=cert.getSignature();
		byte hashForSign[BELT_HASH_LENGHT];
		belt_hash((byte*)tbs.data(),tbs.length(),hashForSign);
		return bign_verify(hashForSign,pubkey, sig, BELT_HASH_LENGHT);
	}
	bool isValid(){
		if(memcmp(trustedCert.pubkey, pubkey, PUBKEY_LENGHT) == 0)
			return true;
		if(issuer == NULL){
			return false;
		}
		return issuer->verify(*this) && issuer->isValid();

	}
	Cert (Cert* issuer, string subject, byte* pubkey, byte* signature)
	{
		this->issuer=issuer;
		this->subject=subject;
		this->pubkey=pubkey;
		this->signature=signature;
	}

	~Cert(void)
	{
		delete[PUBKEY_LENGHT] pubkey;
		delete[BELT_HASH_LENGHT] signature;
	}
	string get_tbsCertificate() const{
		return subject;
	}
	/*static Cert& createCert(string newSubject, byte* newPubKey){
		byte sig[BELT_HASH_LENGHT];
		bign_sign(sig,
		Cert newCert(this,newSubject,newPubKey, sig);
	}*/
};

