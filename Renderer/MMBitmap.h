#pragma once

#include "../framework.h"



struct PIXEL_STREAM
{
	WORD wPosX;
	WORD wPixelNum;
	DWORD dwColor;
};

struct COMPRESSED_PIXEL_LINE
{
	DWORD dwLineDataSize;
	DWORD dwStreamNum;
	PIXEL_STREAM* pPixelStream;
};

struct MMBitmap
{
	DWORD dwWidth;
	DWORD dwHeight;
	COMPRESSED_PIXEL_LINE* pStreamLine;
};