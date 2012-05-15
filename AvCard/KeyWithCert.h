#pragma once
#include "cardobject.h"
#include "cert.h"
class KeyWithCert :
	public CardObject
{
private:
	byte* privKey;
	Cert cert;
public:
	const Cert& getCert() const {
		return cert;
	}
	KeyWithCert(void)
	{
	}

	~KeyWithCert(void)
	{
	}
};

