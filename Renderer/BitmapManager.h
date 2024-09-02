#pragma once

#include "../framework.h"
#include "../Typedef.h"



class CHashTable;

class CBitmapManager
{
public:
	CBitmapManager();
	~CBitmapManager();

	BOOL Initialize(DWORD dwMaxBucketNum);

	BITMAP_HANDLE* CreateBitmap(UINT BmpWidth, UINT BmpHeight);
	BITMAP_HANDLE* CreateBitmapFromBMPFile(const WCHAR* szFileName);
	void DeleteBitmap(BITMAP_HANDLE* pBmp);

	BOOL CreateCompressBitmap(BITMAP_HANDLE* pHBmp, const COLOR* pColorKey);
	BOOL UpdateCompressBitmap(BITMAP_HANDLE* pHBmp, const COLOR* pColorKey);
	void DeleteCompressBitmap(BITMAP_HANDLE* pHBmp);

private:
	void Cleanup();

	BITMAP_HANDLE* AllocBitmapHandle();
	DWORD FreeBitmapHandle(BITMAP_HANDLE* pHBmp);

	BOOL CompressBitmapPerLine(void* pDest, const DWORD* pPixel, DWORD dwLen, const COLOR* pColorKey);

private:
	CHashTable* m_pHashTable = nullptr;
	LINK_NODE* m_pBmpLinkHead = nullptr;
	LINK_NODE* m_pBmpLinkTail = nullptr;
};