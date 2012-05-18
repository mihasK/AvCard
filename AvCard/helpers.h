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

#endif