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

	Cert(void)
	{
	}

	~Cert(void)
	{
	}
};

