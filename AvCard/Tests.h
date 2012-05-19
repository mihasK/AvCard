#pragma once

#include <iostream>
#include "CardObject.h"
#include "RNG.h"
#include "Timer.h"
#include <ctime>

#include "SecureMessaging.h"

void testPACE(){

}
void testTimer(){
	Timer timer=Timer();
	char* time = asctime(timer.get());
	cout << "Current time: " << time << endl;
		system("PAUSE");
}
void testRNG(){
	RNG rng;
	byte * bytes= new byte[100];
	rng.get(bytes,100);
	for(int i=0;i<100;i++)
		cout<<(int)bytes[i]<<" ";
	cout<<endl;
	system("PAUSE");
	delete[] bytes;
}