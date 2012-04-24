#pragma once
#include "cardobject.h"

class CardAuth: CardObject
{
	/*
	policy:
			delegate(activate , response);
			can(set_template , SM::activated());
			can(response , SM::activated());
			todo(deactivate(), when(!SM::activated()));
	*/

private:
	char privKey [32];
public:
	//[cmd:<Set AT >]
	bool set_template(const char * key); //char[32]

	//[cmd:<General Authenticate >]
	char* response(const char * request); //request =char[64], resp - char[24]

	CardAuth(void);
	~CardAuth(void);

			
};

