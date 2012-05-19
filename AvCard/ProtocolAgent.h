#pragma once

#include "typedef.h"
#include "NetAgent.h"
#include <string>
using namespace std;

class ProtocolAgent : public NetAgent
{
public:
	virtual void initiate(){
	}
	
	virtual string getProtocolName();
};

