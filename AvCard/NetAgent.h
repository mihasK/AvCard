#pragma once
#include "typedef.h"
#include <vector>
using namespace std;

class NetAgent
{
private:
	NetAgent* father;
	vector<NetAgent> dauthers;
public:
	void setFather(NetAgent* father){
		this->father = father;
		father->addDauther(*this);
	}
	void addDauther(NetAgent dauther){
		dauthers.push_back(dauther);

	}
	virtual void send(byte* message, int size){
		if(&father != NULL)
			father->send(message, size);
	}
	virtual void receive(byte* message, int size){
		if(&dauthers != NULL){
			for(int i=0;i<dauthers.size();i++)
				dauthers.at(i).receive(message, size);
		}
	}
};

