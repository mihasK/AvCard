#pragma once
#include "typedef.h"
#include "NetAgent.h"
#include <deque>
#include <vector>
using namespace std;

deque<vector<byte>> deq;

class RightNetAgent : public NetAgent{
public :
	void send(byte* message, int size){
		vector<byte> packet(size);
		for(int i=0;i<size;i++)
			packet.push_back(message[i]);
		deq.push_back(packet);
	}
	
	void receive(){
		vector<byte> packet = deq.back();
		deq.pop_back();
		byte* message=new byte[packet.size()];
		for(int i=0;i<packet.size();i++)
			message[i]=packet[i];
		NetAgent::receive(message, packet.size());
		delete[packet.size()] message;
		
	}
};

class LeftNetAgent : public NetAgent{
public :
	void send(byte* message, int size){
		vector<byte> packet(size);
		for(int i=0;i<size;i++)
			packet.push_back(message[i]);
		deq.push_front(packet);
	}
	
	void receive(){
		vector<byte> packet = deq.at(0);
		deq.pop_front();
		byte* message=new byte[packet.size()];
		for(int i=0;i<packet.size();i++)
			message[i]=packet[i];
		NetAgent::receive(message, packet.size());
		delete[packet.size()] message;
		
	}
};




