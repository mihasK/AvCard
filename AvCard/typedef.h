#pragma once

#if _MSC_VER > 1000
#pragma once
#endif 

#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else 
#define EXTERN_C 
#endif
#endif
#ifdef _WINDOWS_
	typedef unsigned __int64 uint64;
	typedef __int64 int64;
#else
	typedef unsigned long long uint64;
	typedef long long int64;
#endif
	
typedef unsigned int    uint32; 
typedef unsigned short  uint16;
typedef unsigned char   uint8;  
typedef int             int32;  
typedef short int       int16;  
typedef signed char	    int8;
typedef unsigned char	byte;


#ifndef _WINDOWS_
typedef int	BOOL;
#endif

#ifndef TRUE
#define TRUE ((BOOL)(1))
#endif

#ifndef FALSE
#define FALSE ((BOOL)(0))
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

 
