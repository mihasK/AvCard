#pragma once
#include <iostream>
#include "CardObject.h"
#include "RNG.h"
#include "Timer.h"
#include <ctime>
#include "tests.h"


using namespace std;

void testTimer(){
	Timer timer=Timer();
	char* time = asctime(timer.get());
	cout << "Current time: " << time << endl;
		system("PAUSE");
}

int main(){
	/*RNG rng;
	cout << "Hello world"<<endl;
	byte * bytes= new byte[100];
	rng.get(bytes,100);
	for(int i=0;i<100;i++)
		cout<<(int)bytes[i]<<" ";
	cout<<endl;
	system("PAUSE");
	delete[] bytes;*/
	tests::belt_test();
	//testTimer();
	return 0;

	
}
