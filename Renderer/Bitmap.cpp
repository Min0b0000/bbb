#include "Bitmap.h"
#include <stdio.h>
#include <stdlib.h>



BOOL CreateBitmap(Bitmap** ppBmp, DWORD dwWidth, DWORD dwHeight)
{
    BOOL bRsl = FALSE;
    Bitmap* pBitmap = NULL;
    DWORD dwSize = sizeof(Bitmap) + (dwWidth * dwHeight * 4);
    pBitmap = (Bitmap*)malloc(dwSize);
    (*ppBmp) = pBitmap;

    if (!pBitmap)
    {
        goto lb_return;
    }

    memset(pBitmap->pixels, 0, dwSize - sizeof(Bitmap));
    bRsl = TRUE;

lb_return:
    return bRsl;
}

BOOL CreateBitmapFromBMP(Bitmap** ppBmp, const WCHAR* wcsFileName)
{
    BOOL bRsl = FALSE;
    Bitmap* pBitmap = NULL;
	FILE* f = NULL;
    DWORD dwSize = 0;
    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;

    _wfopen_s(&f, wcsFileName, L"rb");
    if (NULL == f)
    {
        goto lb_return;
    }

    fread_s(&bf, sizeof(BITMAPFILEHEADER), sizeof(BITMAPFILEHEADER), 1, f);
    if (0x4D42 != bf.bfType)
    {
        goto lb_return;
    }

    fread_s(&bi, sizeof(BITMAPINFOHEADER), sizeof(BITMAPINFOHEADER), 1, f);
    if (32 != bi.biBitCount)
    {
        goto lb_return;
    }

    if (!CreateBitmap(&pBitmap, bi.biWidth, bi.biHeight))
    {
        goto lb_return;
    }
    pBitmap->dwWidth    = bi.biWidth;
    pBitmap->dwHeight   = bi.biHeight;
    
    dwSize = pBitmap->dwWidth * pBitmap->dwHeight * 4;
    fseek(f, bf.bfOffBits, SEEK_SET);
    fread_s( pBitmap->pixels, dwSize, 1, dwSize, f);
    fclose(f);

    FlipBitmap(pBitmap->dwWidth, pBitmap->dwHeight, pBitmap->pixels, FLIP_VERTICAL);

    (*ppBmp) = pBitmap;

    bRsl = TRUE;

lb_return:
    return bRsl;
}

void DeleteBitmap(Bitmap* pBitmap)
{
    free(pBitmap);
}

void FlipBitmap(DWORD dwBitmapWidth, DWORD dwBitmapHeight, BYTE* pPixels, FILP_BITMAP_FLAG flipFlag)
{
    const DWORD DWBUFSIZE = 256;
    DWORD dwBuf[DWBUFSIZE];
    DWORD dwDest0, dwDest1;
    DWORD dwWidth, dwHeight;
    DWORD dwPitch = dwBitmapWidth;
    DWORD* pdwPxl = (DWORD*)pPixels;

    if (FLIP_HORIZENTAL & flipFlag)
    {
        dwWidth = dwBitmapWidth / 2;
        dwHeight = dwBitmapHeight;
        for (DWORD i = 0; dwHeight > i; ++i)
        {
            dwDest0 = 0;
            dwDest1 = dwBitmapWidth - 1;
            for (DWORD j = 0; dwWidth > j; ++j)
            {
                (*dwBuf) = pdwPxl[dwDest0];
                pdwPxl[dwDest0] = pdwPxl[dwDest1];
                pdwPxl[dwDest1] = (*dwBuf);
                dwDest0++;
                dwDest1--;
            }
            pdwPxl += dwPitch;
        }
    }

    if (FLIP_VERTICAL & flipFlag)
    {
        dwWidth = dwBitmapWidth;
        dwHeight = dwBitmapHeight / 2;
        dwDest0 = 0;
        dwDest1 = dwPitch * (dwBitmapHeight - 1);
        if (DWBUFSIZE >= dwWidth)
        {
            dwWidth *= 4;
            pdwPxl = (DWORD*)pPixels;
            for (DWORD i = 0; dwHeight > i; ++i)
            {
                memcpy(dwBuf, pdwPxl + dwDest0, dwWidth);
                memcpy(pdwPxl + dwDest0, pdwPxl + dwDest1, dwWidth);
                memcpy(pdwPxl + dwDest1, dwBuf, dwWidth);
                dwDest0 += dwPitch;
                dwDest1 -= dwPitch;
            }
        } else
        {
            DWORD dwLoop = dwWidth / DWBUFSIZE;
            DWORD dwRawBufSize = DWBUFSIZE * 4;
            pdwPxl = (DWORD*)pPixels;
            for (DWORD i = 0; dwLoop > i; ++i)
            {
                for (DWORD j = 0; dwHeight > j; ++j)
                {
                    memcpy(dwBuf, pdwPxl + dwDest0, dwRawBufSize);
                    memcpy(pdwPxl + dwDest0, pdwPxl + dwDest1, dwRawBufSize);
                    memcpy(pdwPxl + dwDest1, dwBuf, dwRawBufSize);
                    dwDest0 += dwPitch;
                    dwDest1 -= dwPitch;
                }
                dwDest0 = 0;
                dwDest1 = dwPitch * (dwBitmapHeight - 1);
                pdwPxl += DWBUFSIZE;
            }

            dwWidth %= DWBUFSIZE;
            dwWidth *= 4;
            if (!dwWidth)
            {
                return;
            } else for (DWORD i = 0; dwHeight > i; ++i)
            {
                memcpy(dwBuf, pdwPxl + dwDest0, dwWidth);
                memcpy(pdwPxl + dwDest0, pdwPxl + dwDest1, dwWidth);
                memcpy(pdwPxl + dwDest1, dwBuf, dwWidth);
                dwDest0 += dwPitch;
                dwDest1 -= dwPitch;
            }
        }
    }
}
