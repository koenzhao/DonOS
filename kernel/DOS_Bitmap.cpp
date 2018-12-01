#include "DOS_Bitmap.h"

void DOS_32Bitmap_SetBit(uint32_t* pBitmap, uint8_t bit)
{
	if((pBitmap == NULL) || (bit > 31))
	{
		return ;
	}

	*pBitmap = *pBitmap | (1 << bit);
}

void DOS_32Bitmap_ClearBit(uint32_t* pBitmap, uint8_t bit)
{
	if((pBitmap == NULL) || (bit > 31))
	{
		return ;
	}

	*pBitmap = *pBitmap & ~(1 << bit);
}

int8_t DOS_32Bitmap_GetBitState(uint32_t* pBitmap, uint8_t bit)
{
	if((pBitmap == NULL) || (bit > 31))
	{
		return DOS_ERROR;
	}

	uint32_t state;
	state = *pBitmap & (1 << bit);

	if(state == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int16_t DOS_32Bitmap_GetFirstSetBit(uint32_t* pBitmap)
{
	if(pBitmap == NULL)
	{
		return DOS_ERROR;
	}

	uint8_t setBit = 0;

	while(setBit < 32)
	{
		if((*pBitmap & (1 << setBit)) != 0)
		{
			return setBit;
		}
		setBit++;
	}
	return DOS_ERROR;
}


