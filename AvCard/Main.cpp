#include <iostream>
#include "CardObject.h"
#include "RNG.h"
using namespace std;

int main(){
	cout << "Hello world"<<endl;
	RNG rng;
	unsigned char * bytes=rng.get(100);
	for(int i=0;i<100;i++)
		cout<<(int)bytes[i]<<" ";
	cout<<endl;
	system("PAUSE");
	delete[100] bytes;
}