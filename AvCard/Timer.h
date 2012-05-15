#pragma once
#include "cardobject.h"
#include "Timer.h"
#include <iostream>
#include <ctime>

class Timer :
	public CardObject
{
public:
	tm* get();
	Timer(void);
	~Timer(void);
};

