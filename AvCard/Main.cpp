#pragma once
#include <iostream>
#include "Tests.h"
#include <sstream>

using namespace std;


int main(){
	
	tests::belt_test();

	tests::testRNG();
	tests::testTimer();
	//tests::testPACE();



	return 0;


}