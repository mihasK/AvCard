#include "RNG.h"
#include <random>
#include <time.h>
using namespace std;

unsigned char* RNG::get(unsigned count){
	unsigned char* bytes=new unsigned char[count];
	for(int i=0;i<count;i++)
		bytes[i]=(unsigned char)rand();
	return bytes;
}


RNG::RNG(void)
{
	srand((unsigned)time(0));
}


RNG::~RNG(void)
{
}
