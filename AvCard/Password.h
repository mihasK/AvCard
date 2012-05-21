#pragma once
#include "cardobject.h"
class Password :
		public CardObject
{
public:
	byte* data;
	short length;

	Password(void);
	~Password(void);
};
class PIN :public Password{

}

class PUK :public Password{

}

