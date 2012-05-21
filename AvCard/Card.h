#pragma once
#include "Timer.h"
#include "SecureMessaging.h"
#include "ProtocolAgent.h"
#include "KeyWithCert.h"
#include "NetEmul.h"
#include "Ca.h"
#include "Password.h"

class Card : ProtocolAgent
{
private:
	RNG rng;
	Timer timer;
	SMResponser sm;
	CaResponser cardAuth;
	CaRequester termAuth;
	KeyWithCert kcert;
	PIN pin;
	PUK puk;
	ClientAppAgregator apps;
	NetAgent net;
public:
	void send(byte* message, int size){
		if(sm.isActive() == true)
			sm.wrap(message, size, message);
		net.send(message, size);
	}
	void receive(byte* message, int size){
		if(sm.isActive() == true)
			sm.unwrap(message, size, message);
		NetAgent::receive(message, size);
	}
	Card(void);
	~Card(void);
};

