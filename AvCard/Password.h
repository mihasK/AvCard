#pragma once
#include "cardobject.h"
class Password :
		public CardObject
{
private:
	char* data;
	short lenght;

public:
	Password(void);
	~Password(void);
};

