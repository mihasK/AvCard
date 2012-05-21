#pragma once
#include <iostream>
#include "tests.h"
#include <sstream>
using namespace std;


int main(){
	freopen("output.txt", "w", stdout);
	cout << "static const uint16 table[][] = { \n ";

	for (unsigned short  i = 0; i < 256; ++i) {
		cout << "{ ";
		for (unsigned short  j = 0; j < 256; ++j) {
			unsigned short ret = 0;
			unsigned short  ii = i, jj = j;
			for (int a = 0; a < 8; ++a) {
				if ((ii >> a) & 1) ret ^= jj << a;
			}

			stringstream ss;
			ss << hex << ret;
			string t;
			ss >> t;
			while (t.length() != 4) t = "0" + t;
			for (int i = 0; i < t.length(); ++i) t[i] = toupper(t[i]);
			cout << "0x" << t;
			if (j != 255) cout << ", ";
			else { cout << " "; }
		}
		cout << "} ,\n";
	}
	cout << " };";
	tests::belt_test();
//	tests::testRNG();
	/*tests::testTimer();
	tests::testPACE();
*/

	return 0;


}