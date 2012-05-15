#include "Timer.h"
#include <iostream>
#include <ctime>

using namespace std;

tm* Timer::get(){
	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	return timeinfo;

}

Timer::Timer(void)
{
}


Timer::~Timer(void)
{
}
