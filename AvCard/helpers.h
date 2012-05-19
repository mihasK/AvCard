#ifndef __HELPERS_H_
#define __HELPERS_H_
#include "point.h"

EXTERN_C void swu(byte *X, uint32 size, byte* P){};


uint32 getSize(const uint32 &source) {
	return source >> 5;
}
uint32 getIndex(const uint32 &source) {
	return source & 0x1F;
}
bool getBit(const uint32 * data, const uint32 &at) {
		uint32 _at = getSize(at), _$ = getIndex(at);
		return (data[_at] & (1U << _$)) != 0U;
	}
	void setBit(uint32 * data, const uint32& at, const bool val = TRUE) {
		uint32 _at = getSize(at),
			_val = val ? (1U << getIndex(at)) : ~(1U<<getIndex(at));
		data[_at] &= _val;
	}
	void toggleBit(uint32 * data, const uint32& at) {
		bool val = getBit(data, at);
		setBit(data, at, !val);
	}

#endif