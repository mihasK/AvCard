#include "RNG.h"
#include <random>
#include <time.h>
using namespace std;

byte* RNG::get(unsigned count){
	byte* bytes=new byte[count];
	for(int i=0;i<count;i++)
		bytes[i]=(byte)rand();
	return bytes;
}


RNG::RNG(void)
{
	srand((unsigned)time(0));
}


RNG::~RNG(void)
{
}
