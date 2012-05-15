#include <iostream>
#include "CardObject.h"
#include "RNG.h"
#include "Timer.h"
#include "Cert.h"
#include <ctime>

using namespace std;

void testTimer(){
	Timer timer=Timer();
	char* time = asctime(timer.get());
	cout << "Current time: " << time << endl;
		system("PAUSE");
}

int main(){
	RNG rng;
	cout << "Hello world"<<endl;
	unsigned char * bytes=rng.get(100);
	for(int i=0;i<100;i++)
		cout<<(int)bytes[i]<<" ";
	cout<<endl;
	system("PAUSE");
	delete[] bytes;
	testTimer();

	
}
