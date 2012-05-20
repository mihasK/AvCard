#pragma once
#include "point.h"
static void swu(byte *X, uint32 size, byte* P){};


static uint32 getSize(const uint32 &source) {
	return source >> 5;
}
 static uint32 getIndex(const uint32 &source) {
	return source & 0x1F;
}
 static bool getBit(const uint32 * data, const uint32 &at) {
		uint32 _at = getSize(at), _$ = getIndex(at);
		return (data[_at] & (1U << _$)) != 0U;
	}
static void setBit(uint32 * data, const uint32& at, const bool val = TRUE) {
		uint32 _at = getSize(at),
			_val = val ? (1U << getIndex(at)) : ~(1U<<getIndex(at));
		data[_at] &= _val;
	}
static 	 void toggleBit(uint32 * data, const uint32& at) {
		bool val = getBit(data, at);
		setBit(data, at, !val);
	}

static void change_endian(byte *X) {
	byte*l = X, 
			*r = X + 3;

		*r ^= *l, *l ^= *r, *r ^= *l;
		l = X + 1, 
			r = X + 2;

		*r ^= *l, *l ^= *r, *r ^= *l;

}