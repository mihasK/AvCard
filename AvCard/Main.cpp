#pragma once
#include <iostream>
#include "CardObject.h"
#include "RNG.h"
#include "Timer.h"
#include <ctime>
#include "tests.h"


using namespace std;


int main(){
	tests::belt_test();
	tests::testRNG();
	tests::testTimer();
	tests::testPACE();
	return 0;


}