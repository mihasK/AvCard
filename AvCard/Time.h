#pragma once
#include "cardobject.h"
class Time :
	public CardObject
{
public:
	char* get();
	Time(void);
	~Time(void);
};

