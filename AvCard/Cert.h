#pragma once
#include "cardobject.h"
class Cert :
	public CardObject
{
public:
	//byte[64]
	byte* pubkey() const {
		
	}
	//byte[64]
	byte* pubkey_ri() const  {
	}
	byte* issuer() const  {
	}
	byte* subject() const  {
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
		if (subject() != cert.issuer())
			return false;
		/*if (Timer::date() > cert.date())
			return false;
		разобрать cert, выделить подписываемую часть (tbsCertificate);
		разобрать cert, выделить Ё÷ѕ (signatureValue);
		if (!bign_verify(belt_hash(tbsCertificate),
		signatureValue , pubkey())
		return false;*/
		return true;
	}
	Cert(void)
	{
	}

	~Cert(void)
	{
	}
};

