#ifndef _DOS_COMMONDEFINE_H_
#define _DOS_COMMONDEFINE_H_

#include <stdbool.h>
#include <stdio.h>
//#include <memory.h>
#include <string.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef __stdint_h

#ifndef uint32_t
#define uint32_t unsigned int
#endif

#ifndef int32_t
#define int32_t  int
#endif

#ifndef uint16_t
#define uint16_t unsigned short
#endif

#ifndef int16_t
#define int16_t  short
#endif

#ifndef uint8_t
#define uint8_t  unsigned char
#endif

#ifndef int8_t
#define int8_t   char
#endif

#endif

typedef enum _DOS_STATUS
{
	DOS_ERROR   = -1,
	DOS_FAIL    = -2,
	DOS_SUCCESS =  1
} DOS_STATUS;

#define vuint32_t volatile unsigned long
#define vuint8_t  volatile unsigned char

#define MEM32(addr)         *(volatile unsigned long *)(addr)
#define MEM8(addr)          *(volatile unsigned char *)(addr)

#endif
