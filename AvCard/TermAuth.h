#pragma once
#include "cardobject.h"
class TermAuth :
	public CardObject
{
		/*
	policy:
		delegate(activate , auth);
		can(set_template , SM::activated());
		can(request , SM::activated());
		can(auth , SM::activated());
		todo(deactivate(), when(!SM::activated()));
		};
		*/

private :
	//secret session
	char rb [16];
	char yb [48];
public:

	//[cmd:<MSE: Set AT >]
	bool set_template();

	//[cmd:<Get Challenge >]
	char* request(); //char[64]

	//[cmd:<External Authenticate >]
	bool auth(const char* response); //char[24]
	TermAuth(void);
	~TermAuth(void);

		
}
