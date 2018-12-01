#ifndef _DOS_BITMAP_H_
#define _DOS_BITMAP_H_

#include "DOS_CommonDefine.h"

extern void    DOS_32Bitmap_SetBit(uint32_t* pBitmap, uint8_t bit);
extern void    DOS_32Bitmap_ClearBit(uint32_t* pBitmap, uint8_t bit);
extern int8_t  DOS_32Bitmap_GetBitState(uint32_t* pBitmap, uint8_t bit);
extern int16_t DOS_32Bitmap_GetFirstSetBit(uint32_t* pBitmap);

#endif
