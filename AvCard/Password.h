#pragma once
#include "cardobject.h"
class Password :
		public CardObject
{
public:
	byte* data;
	short lenght;

	Password(void);
	~Password(void);
};

