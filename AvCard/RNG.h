#pragma once
#include "CardObject.h"
#include <random>
#include <time.h>


class RNG : public CardObject
{
public:
	void get(byte* to,unsigned count){
		for(int i=0;i<count;i++)
			to[i]=(byte)rand();
	}


	RNG(void)
	{
		srand((unsigned)time(0));
	}

	~RNG(void){};
};

