#pragma once

#include "typedef.h";

class CardObject
{
private:
	bool active;
public:
	virtual void activate(){
		active=true;
	}
	virtual void deactivate(){
		active=false;
	}
	CardObject(void)
	{
		active=false;
	}

	~CardObject(void)
	{
	}
};

