#pragma once

#include "../framework.h"



struct Bitmap
{
	DWORD	dwWidth;
	DWORD	dwHeight;
	BYTE	pixels[0];
};

enum FILP_BITMAP_FLAG
{
	FLIP_NONE			= 0x0000,
	FLIP_HORIZENTAL		= 0x0001,
	FLIP_VERTICAL		= 0x0002,
};

BOOL CreateBitmap(Bitmap** ppBmp, DWORD dwWidth, DWORD dwHeight);
BOOL CreateBitmapFromBMP(Bitmap** ppBmp, const WCHAR* wcsFileName);
void DeleteBitmap(Bitmap* pBitmap);
void FlipBitmap(DWORD dwWidth, DWORD dwHeight, BYTE* pPixels, FILP_BITMAP_FLAG flipFlag);