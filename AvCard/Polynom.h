#ifndef __POLYNOM_H_
#define __POLYNOM_H_
#include "typedef.h"
#include <cstdlib>
#include <cstring>
#include "helpers.h"
#pragma once
#define POLY_MAX_LEN 4
#define MAX_VALUE_32 (((1ULL) << 32) - 1)
static uint16 table[1 << 8][1 << 8];
#define TABLE_SIZE 0xFF
class Polynom {
	uint32 data[POLY_MAX_LEN + 1];
	uint32 degree;
	enum  __{
		MAX_DEGREE = 159,
	};

	void doit() {
		this->degree = MAX_DEGREE;
		while (this->degree >= 0 && !getBit(this->degree)) --this->degree;
	}

	

	template<uint32 N> void mul_table(const uint32* a, const uint32* b, uint32* c) {
		byte *A = (byte*)a;
		byte *B = (byte*)b;
		uint16 *C = (uint16*)c;
		uint32 sz = (N - 1) << 1;
		for (size_t at = 0; at < N; ++at) {
			size_t at2 = sz - at;
			C[at] = 0;
			for (size_t i = 0; i <= at; ++i) {
				C[at] ^= table[A[i]][B[at-i]];
			}
			if (at2 != at) {
				C[at2] = 0;
				for (size_t i = 0; i <= at; ++i) {
					C[at2] ^= table[A[N - 1 - i]][B[N - 1 - (at - i)]];	
				}
			}
		}
	}

	template<int N > void doMul(uint32 *A, uint32 *B, uint32 *C) {
		if (N == 1) {
			mul_table<4>(A, B, C);
			return;
		}

		uint32 k = N >> 1;
		uint32 A1[N >> 1], A2[N >> 1], B1[N >> 1], B2[N >> 1];
		for (size_t i = 0; i < k; ++i)  {
			A1[i] = A[i], A2[i] = A[k + i],
			B1[i] = B[i], B2[i] = B[k + i];
		}
		uint32 A1B1[N], A2B2[N];
		doMul<N / 2>(A1, B1, A1B1);
		doMul<N / 2>(A2, B2, A2B2);
		for (size_t i = 0; i < k; ++i) {
			A2[i] ^= A1[i],
			B2[i] ^= B1[i];
		}
		uint32 R[N];
		doMul<N / 2>(A2, B2, R);
		for (size_t i = 0; i < N; ++i) 
			R[i] ^= A1B1[i] ^ A2B2[i];
		
        for (size_t i = 0; i < N; i++) {
            C[i + k] ^= r[i],
			C[i] ^= A1B1[i],
			C[i + N] ^= A2B2[i];
		}
	}

	void reduce(uint32 * data, uint32 at) {
		toggleBit(data, at);
		toggleBit(data, at - 128);
		toggleBit(data, at - 127);
		toggleBit(data, at - 126);
		toggleBit(data, at - 121);
	}
	//x ^ 128 + x ^ 7 + x ^ 2 + x ^ 1 + x ^ 0

	uint32 mul_total(uint32 *A, uint32 *B, uint32 * C) {
		doMul<4>(A, B, C);
		uint32 _at = 255;
		while (!::getBit(C, _at)) --_at;
		for (; _at > 127; --_at) {
			if (::getBit(C, _at)) {
				reduce(C, _at);
			}
		}
		while (_at >= 0 && !::getBit(C, _at)) --_at;
		return _at;
	}

	

public:
	Polynom(byte *data, uint32 length) {
		uint32 len = 0;
		memset(this->data, 0, sizeof this->data);
		for (int at = length - 1; at >= 0; at -= 4) {
			for (int j = max(0, at - 3); j <= at; ++j) {
				this->data[len] <<= (sizeof byte) << 3;
				this->data[len] |= data[j];
			}
			++len;
		}
		this->degree = MAX_DEGREE;
		doit();

	}

	bool getBit(const uint32 &at) {
		return ::getBit(this->data, at);
	}

	void setBit(const uint32& at, const bool val = TRUE) {
		::setBit(this->data, at, val);
	}

	Polynom& operator+=(const Polynom &another) {
		for (size_t at = 0; at < POLY_MAX_LEN; ++at) this->data[at] ^= another.data[at];
		doit();
		return *this;
	}

	Polynom& operator-=(const Polynom &another) {
		for (size_t at = 0; at < POLY_MAX_LEN; ++at) this->data[at] ^= another.data[at];
		doit();
		return *this;
	}

	Polynom & operator *= (const Polynom &another) {
		uint32 A[4], B[4];
		memcpy(A, this->data, sizeof A);
		memcpy(B, another.data, sizeof B);
		uint32 C[8];
		this->degree = mul_total(A, B, C);
		memcpy(this->data, C, sizeof this->data);
	}
};

#endif