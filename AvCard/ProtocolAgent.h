#pragma once

#include "typedef.h"
#include <string>
using namespace std;

class ProtocolAgent
{
public:
	virtual void send(byte* message, int size);
	virtual void receive(byte* message, int size);
	virtual string getProtocolName();
};

