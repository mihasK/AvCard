#pragma once
#include "CardObject.h"


class RNG : public CardObject
{
public:

	byte* get(unsigned count);
	RNG(void);
	~RNG(void);
};

