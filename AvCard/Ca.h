#pragma once
#include "cardobject.h"
#include "ProtocolAgent.h"
class Ca :
	public CardObject, public ProtocolAgent
{
public:
	Cert& cert;
	string getProtocolName(){
		return "CA";
	}
	Ca(void);
	~Ca(void);
};

class CaResponser :
	public Ca
{
public:
	void response(byte* message, int size);
	Ca(void);
	~Ca(void);
};

class CaRequester :
	public Ca
{
public:
	void request(byte* message, int size);
	Ca(void);
	~Ca(void);
};
