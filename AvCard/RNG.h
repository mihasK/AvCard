#pragma once
#include "CardObject.h"


class RNG : public CardObject
{
public:

	unsigned char* get(unsigned count);
	RNG(void);
	~RNG(void);
};

