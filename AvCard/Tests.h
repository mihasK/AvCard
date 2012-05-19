#pragma once
#include "belt.h"
#include <string>
#include <fstream>
#include <cassert>
#include "SecureMessaging.h"
#include "NetEmul.h"
using namespace std;

namespace tests {
	void testPACE(){
		SMRequester requester;
		SMResponser responser;
		RightNetAgent rAgent;
		LeftNetAgent lAgent;
		requester.setFather(&rAgent);
		responser.setFather(&lAgent);
		
		requester.initiate();
		lAgent.receive();
		rAgent.receive();
		lAgent.receive();
		rAgent.receive();
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

	static void output_correct_bytes(string &bytes, string file, string name) {
		ofstream fout(file);
		assert(bytes.length() % 2 == 0);
		fout << "byte " << name << "[] = {\n";
		for (int i = 0; i < bytes.length(); i += 2) {
			fout << "0x" << bytes[i] << bytes[i + 1] << ",\n";
		}
		fout << "};";
		fout.close();

	}

	static void belt_test() {
		byte X[] = {
			0xB1,
			0x94,
			0xBA,
			0xC8,
			0x0A,
			0x08,
			0xF5,
			0x3B,
			0x36,
			0x6D,
			0x00,
			0x8E,
			0x58,
			0x4A,
			0x5D,
			0xE4
		};

		byte SIGMA[] = {
			0xE9,
			0xDE,
			0xE7,
			0x2C,
			0x8F,
			0x0C,
			0x0F,
			0xA6,
			0x2D,
			0xDB,
			0x49,
			0xF4,
			0x6F,
			0x73,
			0x96,
			0x47,
			0x06,
			0x07,
			0x53,
			0x16,
			0xED,
			0x24,
			0x7A,
			0x37,
			0x39,
			0xCB,
			0xA3,
			0x83,
			0x03,
			0xA9,
			0x8B,
			0xF6
		};

		byte Y[] = {
			0x69,
			0xCC,
			0xA1,
			0xC9,
			0x35,
			0x57,
			0xC9,
			0xE3,
			0xD6,
			0x6B,
			0xC3,
			0xE0,
			0xFA,
			0x88,
			0xFA,
			0x6E
		};
		uint32 size = sizeof X;
		byte *my_y = new byte[size];
		belt_ecb_encr(X, size, SIGMA, my_y);
		for (int i = 0; i < size; ++i) assert(Y[i] == my_y[i]);
		byte *my_x = new byte[size];
		belt_ecb_decr(Y, size, SIGMA, my_x);
		for (int i = 0; i < size; ++i) assert(X[i] == my_x[i]);
		byte x_hash[] = {0xB1,0x94,0xBA,0xC8 ,0x0A,0x08,0xF5,0x3B ,0x36,0x6D,0x00 ,0x8E ,0x58};
		byte y_hash[512];
		belt_hash(x_hash, sizeof x_hash, y_hash);
		uint32 t = 0;
		t <<= 4;
		uint32 q = t + 2;
	}

};