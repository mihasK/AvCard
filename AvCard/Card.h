#pragma once
#include "Timer.h"
#include "SecureMessaging.h"
#include "ProtocolAgent.h"

class Card : ProtocolAgent
{
private:
	Timer timer;
	SMResponser sm;
public:
	void send(byte* message, int size);
	void receive(byte* message, int size);
	Card(void);
	~Card(void);
};

