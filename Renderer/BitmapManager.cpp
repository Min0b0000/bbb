#include "BitmapManager.h"
#include "../Util/HashTable.h"



CBitmapManager::CBitmapManager()
{
}

CBitmapManager::~CBitmapManager()
{
    Cleanup();
}

BOOL CBitmapManager::Initialize(DWORD dwMaxBucketNum)
{
    BOOL bRsl = FALSE;

    m_pHashTable = new CHashTable;
    m_pHashTable->Initialize(dwMaxBucketNum, _MAX_PATH);

    bRsl = TRUE;

lb_return:
    return bRsl;
}

BITMAP_HANDLE* CBitmapManager::CreateBitmap(UINT BmpWidth, UINT BmpHeight)
{
	BITMAP_HANDLE* pHBmp = AllocBitmapHandle();
	if (::CreateBitmap(&pHBmp->pBitmap, BmpWidth, BmpHeight))
	{
		__debugbreak();
	}
    return pHBmp;
}

BITMAP_HANDLE* CBitmapManager::CreateBitmapFromBMPFile(const WCHAR* szFileName)
{
	Bitmap* pBmp = nullptr;
	BITMAP_HANDLE* pHbmp = nullptr;
	DWORD dwFileNameLen = (DWORD)wcslen(szFileName);
	DWORD dwKeySize = dwFileNameLen * sizeof(WCHAR);

	if (m_pHashTable->Select((void**)&pHbmp, 1, szFileName, dwKeySize))
	{
		pHbmp->dwRefCount++;
	} else
	{
		if (::CreateBitmapFromBMP(&pBmp, szFileName))
		{
			pHbmp = AllocBitmapHandle();
			pHbmp->pBitmap = pBmp;

			pHbmp->pSearchHandle = m_pHashTable->Insert((void*)pHbmp, szFileName, dwKeySize);
			if (!pHbmp->pSearchHandle)
			{
				__debugbreak();
			}
		}
	} return pHbmp;
}

void CBitmapManager::DeleteBitmap(BITMAP_HANDLE* pBmp)
{
	FreeBitmapHandle(pBmp);
}

BOOL CBitmapManager::CreateCompressBitmap(BITMAP_HANDLE* pHBmp, const COLOR* pColorKey)
{
	BOOL bRsl = FALSE;
	MMBitmap* pCpBmp = nullptr;
	DWORD dwSize;
	COMPRESSED_PIXEL_LINE* pCPL;

	if (!pHBmp)
	{
		__debugbreak();
	}
	if (pHBmp->pCompressedBitmap)
	{
		goto lb_return;
	}

	pCpBmp = (MMBitmap*)malloc(sizeof(MMBitmap));
	memset(pCpBmp, 0, sizeof(MMBitmap));
	pCpBmp->dwWidth				= pHBmp->pBitmap->dwWidth;
	pCpBmp->dwHeight			= pHBmp->pBitmap->dwHeight;

	dwSize = sizeof(COMPRESSED_PIXEL_LINE) * pCpBmp->dwHeight;
	pCPL = (COMPRESSED_PIXEL_LINE*)malloc(dwSize);
	memset(pCPL, 0, dwSize);
	pCpBmp->pStreamLine = pCPL;

	pHBmp->pCompressedBitmap = pCpBmp;

	UpdateCompressBitmap(pHBmp, pColorKey);

	bRsl = TRUE;

lb_return:
	return bRsl;
}

BOOL CBitmapManager::UpdateCompressBitmap(BITMAP_HANDLE* pHBmp, const COLOR* pColorKey)
{
	BOOL bRsl = FALSE;
	COMPRESSED_PIXEL_LINE* pCPL;
	const DWORD* pSrc;
	DWORD dwSize;
	if (!pHBmp)
	{
		__debugbreak();
	}
	if (!pHBmp->pCompressedBitmap)
	{
		goto lb_return;
	}

	pCPL	= pHBmp->pCompressedBitmap->pStreamLine;
	pSrc	= (DWORD*)pHBmp->pBitmap->pixels;
	dwSize	= pHBmp->pBitmap->dwWidth;

	if (dwSize != pHBmp->pCompressedBitmap->dwWidth || pHBmp->pBitmap->dwHeight != pHBmp->pCompressedBitmap->dwHeight)
	{
		DeleteCompressBitmap(pHBmp);
		CreateCompressBitmap(pHBmp, pColorKey);
		bRsl = TRUE;
		goto lb_return;
	}

	for (DWORD i = 0; pHBmp->pCompressedBitmap->dwHeight > i; ++i)
	{
		CompressBitmapPerLine((void*)pCPL, pSrc, dwSize, pColorKey);
		pCPL++;
		pSrc += dwSize;
	}

	bRsl = TRUE;

lb_return:
	return bRsl;
}

void CBitmapManager::DeleteCompressBitmap(BITMAP_HANDLE* pHBmp)
{
	MMBitmap* pCpBmp = pHBmp->pCompressedBitmap;
	if (!pCpBmp)
	{
		return;
	}

	COMPRESSED_PIXEL_LINE* pCPL = pCpBmp->pStreamLine;
	if (pCPL)
	{
		for (DWORD i = 0; pCpBmp->dwHeight > i; ++i)
		{
			if (pCPL->pPixelStream)
			{
				free(pCPL->pPixelStream);
			}
			pCPL++;
		}
		free(pCpBmp->pStreamLine);
	}
	free(pCpBmp);
	pHBmp->pCompressedBitmap = nullptr;
}

void CBitmapManager::Cleanup()
{
	if (m_pBmpLinkHead)
	{
		__debugbreak();
	}
	if (m_pHashTable)
	{
		delete m_pHashTable;
		m_pHashTable = nullptr;
	}
}

BITMAP_HANDLE* CBitmapManager::AllocBitmapHandle()
{
	BITMAP_HANDLE* pHBmp = (BITMAP_HANDLE*)malloc(sizeof(BITMAP_HANDLE));
	memset(pHBmp, 0, sizeof(BITMAP_HANDLE));
	pHBmp->linkNode.pData = pHBmp;
	LinkToLinkedListFIFO(&m_pBmpLinkHead, &m_pBmpLinkTail, &pHBmp->linkNode);
	pHBmp->dwRefCount = 1;
	return pHBmp;
}

DWORD CBitmapManager::FreeBitmapHandle(BITMAP_HANDLE* pHBmp)
{
	DWORD dwRef = --pHBmp->dwRefCount;
	if (!dwRef)
	{
		if (pHBmp->dwRefCount)
		{
			__debugbreak();
		}
		if (pHBmp->pBitmap)
		{
			::DeleteBitmap((Bitmap*)pHBmp->pBitmap);
			pHBmp->pBitmap = nullptr;
		}
		if (pHBmp->pCompressedBitmap)
		{
			DeleteCompressBitmap(pHBmp);
			pHBmp->pCompressedBitmap = nullptr;
		}
		if (pHBmp->pSearchHandle)
		{
			m_pHashTable->Delete(pHBmp->pSearchHandle);
			pHBmp->pSearchHandle = nullptr;
		}
		UnlinkLinkedList(&m_pBmpLinkHead, &m_pBmpLinkTail, &pHBmp->linkNode);
		free(pHBmp);
	} return dwRef;
}

BOOL CBitmapManager::CompressBitmapPerLine(void* pDest, const DWORD* pPixel, DWORD dwLen, const COLOR* pColorKey)
{
	BOOL bRsl = FALSE;
	COMPRESSED_PIXEL_LINE* pCPL = (COMPRESSED_PIXEL_LINE*)pDest;

	PIXEL_STREAM* pPS = nullptr;
	DWORD dwStreamCount = 0;
	const DWORD* pdwSrc = pPixel;
	DWORD dwPrvColor = pdwSrc[0];
	DWORD dwSize;

	if (pColorKey)
	{
		goto lb_has_color_key;
	}

	dwStreamCount++;
	for (DWORD i = 1; dwLen > i; ++i)
	{
		if (dwPrvColor != pdwSrc[i])
		{
			dwStreamCount++;
			dwPrvColor = pdwSrc[i];
		}
	}

	dwSize = sizeof(PIXEL_STREAM) * dwStreamCount;
	if (pCPL->dwLineDataSize < dwSize)
	{
		if (pCPL->pPixelStream)
		{
			free(pCPL->pPixelStream);
		}
		pCPL->pPixelStream = (PIXEL_STREAM*)malloc(dwSize);
		pCPL->dwLineDataSize = dwSize;
	}

	pCPL->dwStreamNum = dwStreamCount;

	pPS = pCPL->pPixelStream;
	dwPrvColor = pdwSrc[0];
	pPS->wPixelNum++;
	pPS->wPosX = 0;
	pPS->dwColor = pdwSrc[0];
	for (DWORD i = 1; dwLen > i; ++i)
	{
		if (dwPrvColor != pdwSrc[i])
		{
			pPS++;
			pPS->dwColor = pdwSrc[i];
			pPS->wPixelNum++;
			pPS->wPosX = i;
			dwPrvColor = pdwSrc[i];
		} else
		{
			pPS->wPixelNum++;
		}
	}

	bRsl = TRUE;
	goto lb_return;

lb_has_color_key:

	if (pColorKey->dwColorCode != dwPrvColor)
	{
		dwStreamCount++;
	}
	
	for (DWORD i = 1; dwLen > i; ++i)
	{
		if (pColorKey->dwColorCode == pdwSrc[i])
		{
			dwPrvColor = pdwSrc[i];
			continue;
		}
		if (dwPrvColor != pdwSrc[i])
		{
			dwStreamCount++;
			dwPrvColor = pdwSrc[i];
		}
	}

	dwSize = sizeof(PIXEL_STREAM) * dwStreamCount;
	if (pCPL->dwLineDataSize < dwSize)
	{
		if (pCPL->pPixelStream)
		{
			free(pCPL->pPixelStream);
		}
		pCPL->pPixelStream = (PIXEL_STREAM*)malloc(dwSize);
		memset(pCPL->pPixelStream, 0, dwSize);
		pCPL->dwLineDataSize = dwSize;
	}

	pCPL->dwStreamNum = dwStreamCount;

	pPS = NULL;
	dwPrvColor = pColorKey->dwColorCode;
	for (DWORD i = 0; dwLen > i; ++i)
	{
		if (pColorKey->dwColorCode == pdwSrc[i])
		{
			dwPrvColor = pdwSrc[i];
			continue;
		}
		if (dwPrvColor != pdwSrc[i])
		{
			if (!pPS)
			{
				pPS = pCPL->pPixelStream;
			} else
			{
				pPS++;
			}
			pPS->dwColor = pdwSrc[i];
			pPS->wPixelNum++;
			pPS->wPosX = i;
			dwPrvColor = pdwSrc[i];
		} else
		{
			pPS->wPixelNum++;
		}
	}

	bRsl = TRUE;

lb_return:
	return bRsl;
}
