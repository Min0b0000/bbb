#include "DDrawRenderer.h"
#pragma comment(lib, "DDraw.lib")
#pragma comment(lib, "dxguid.lib")
#include <ddraw.h>
#include "SpriteObject.h"
#include "BitmapManager.h"
#include "Bitmap.h"
#include "MMBitmap.h"

#define __DrawDot(x, y, pColor) (*(DWORD*)(m_pLockedBackBuf + (((y) * m_dwLockedBackBufPitch) + (4 * (x))))) = (*(DWORD*)(pColor))



CDDrawRenderer::CDDrawRenderer(void)
{
}

CDDrawRenderer::~CDDrawRenderer(void)
{
	Cleanup();
}

HRESULT CDDrawRenderer::Initialize(HWND hWnd)
{
    HRESULT hr = S_OK;
    DDSURFACEDESC2  ddsd;
    DWORD dwWidth, dwHeight;

    m_hWnd = hWnd;

    ddsd = {};
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    hr = DirectDrawCreate(NULL, &m_pDDraw, NULL);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    hr = m_pDDraw->QueryInterface(IID_IDirectDraw7, (LPVOID*)&m_pDDraw7);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    hr = m_pDDraw7->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    hr = m_pDDraw7->CreateSurface(&ddsd, &m_pDDrawFront, NULL);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    hr = m_pDDraw7->CreateClipper(0, &m_pDDrawClipper, NULL);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    m_pDDrawClipper->SetHWnd(0, hWnd);
    m_pDDrawFront->SetClipper(m_pDDrawClipper);

    m_pBitmapManager = new CBitmapManager;
    m_pBitmapManager->Initialize(BITMAPMANAGER_MAX_BUCKUT_NUM);

    UpdateWindowPos();

    dwWidth = m_rcWindow.right - m_rcWindow.left;
    dwHeight = m_rcWindow.bottom - m_rcWindow.top;

    hr = CreateBackBuffer(dwWidth, dwHeight);
    if (FAILED(hr))
    {
        DeleteBackBuffer();
        goto lb_return;
    }

lb_return:
    return hr;
}

void CDDrawRenderer::UpdateWindowPos(void)
{
    GetClientRect(m_hWnd, &m_rcWindow);
    ClientToScreen(m_hWnd, (POINT*)&m_rcWindow.left);
    ClientToScreen(m_hWnd, (POINT*)&m_rcWindow.right);
}

void CDDrawRenderer::UpdateWindowSize(void)
{
    HRESULT hr;
    DWORD dwWidth, dwHeight;
    UpdateWindowPos();

    dwWidth     = m_rcWindow.right - m_rcWindow.left;
    dwHeight    = m_rcWindow.bottom - m_rcWindow.top;

    DeleteBackBuffer();
    hr = CreateBackBuffer(dwWidth, dwHeight);
    if (FAILED(hr))
    {
        // __debugbreak();
    }
}

HRESULT CDDrawRenderer::CreateSubBuffer(WORD width, WORD height)
{
    HRESULT hr;
    DDSURFACEDESC2 ddsd;

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth = width;
    ddsd.dwHeight = height;

    hr = m_pDDraw7->CreateSurface(&ddsd, &m_pDDrawSub, NULL);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    hr = m_pDDrawSub->GetSurfaceDesc(&ddsd);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    m_dwWidth = ddsd.dwWidth;
    m_dwHeight = ddsd.dwHeight;
    m_rcMask.right = m_dwWidth - 1;
    m_rcMask.bottom = m_dwHeight - 1;

lb_return:
    return hr;
}

void CDDrawRenderer::DeleteSubBuffer(void)
{
    HRESULT hr;
    DDSURFACEDESC2 ddsd;

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

    if (m_pDDrawSub)
    {
        m_pDDrawSub->Release();
        m_pDDrawSub = nullptr;

        hr = m_pDDrawBack->GetSurfaceDesc(&ddsd);
        if (FAILED(hr))
        {
            __debugbreak();
        }

        m_dwWidth = ddsd.dwWidth;
        m_dwHeight = ddsd.dwHeight;
        m_rcMask.right = m_dwWidth - 1;
        m_rcMask.bottom = m_dwHeight - 1;
    }
}

BOOL CDDrawRenderer::BeginDraw(void)
{
    BOOL bResult = TRUE;
    HRESULT hr;
    DWORD dwBufW, dwBufH;

    hr = m_pDDrawFront->IsLost();
    if (DD_OK != hr)
    {
        m_pDDrawFront->Restore();
        goto lb_failed;
    }

    bResult = LockBackBuffer(&m_pLockedBackBuf, &dwBufW, &dwBufH, &m_dwLockedBackBufPitch);
    if (!bResult)
    {
        goto lb_failed;
    } return bResult;

lb_failed:
    bResult = FALSE;
    m_pLockedBackBuf = nullptr;
    return bResult;
}

void CDDrawRenderer::EndDraw(void)
{
    HRESULT hr;
    if (!m_pLockedBackBuf)
    {
        return;
    }

    UnlockBackBuffer();
    if (m_pDDrawSub)
    {
        hr = m_pDDrawBack->Blt(NULL, m_pDDrawSub, NULL, DDBLT_WAIT, NULL);
    }

    m_pDDrawFront->Blt(&m_rcWindow, m_pDDrawBack, NULL, DDBLT_WAIT, NULL);
}

void CDDrawRenderer::DrawClear(const COLOR* pColor)
{
    // DWORD cd = (DWORD)pColor->B + (DWORD)pColor->B - (DWORD)pColor->G - (DWORD)pColor->R;// Color Difference
    DWORD cd;
#ifdef BEGINDRAW_FALSE_DEFEND
    if (!m_pLockedBackBuf)
    {
        return;
    }
#endif

    // if (cd)
    // {
    DWORD* pSample = (DWORD*)m_pLockedBackBuf;
    for (DWORD i = 0; m_dwWidth > i; ++i)
    {
        pSample[i] = (*(DWORD*)pColor);
    }

    cd = 4 * m_dwWidth;
    for (DWORD y = 1; m_dwHeight > y; ++y)
    {
        memcpy(m_pLockedBackBuf + y * m_dwLockedBackBufPitch, pSample, cd);
    }
    // } else
    // {
    //     cd = 4 * m_dwWidth;
    //     for (DWORD y = 0; m_dwHeight > y; ++y)
    //     {
    //         memset(m_pLockedBackBuf + y * m_dwLockedBackBufPitch, pColor->B, cd);
    //     }
    // }
}

void CDDrawRenderer::DrawPixel(DWORD x, DWORD y, const COLOR* pColor)
{
    BOOL bClip = FALSE;
#ifdef BEGINDRAW_FALSE_DEFEND
    if (!m_pLockedBackBuf)
    {
        return;
    }
#endif

    // PointInRect
    if (0 > x)
    {
        goto lb_exit;
    }
    if (m_dwWidth <= x)
    {
        goto lb_exit;
    }
    if (0 > y)
    {
        goto lb_exit;
    }
    if (m_dwHeight <= y)
    {
        goto lb_exit;
    }
    __DrawDot(x, y, pColor);

lb_exit:
    return;
}

void CDDrawRenderer::DrawRectEdge(const RECT* pRect, const COLOR* pEdge)
{
    WORD wIsClipped;
    RECT rect;
    DWORD dwWidth, dwHeight;
    DWORD* pDest[2];

#ifdef BEGINDRAW_FALSE_DEFEND
    if (!m_pLockedBackBuf)
    {
        return;
    }
#endif

    if (!QueryClippingRect(pRect, &m_rcMask, &wIsClipped, &rect))
    {
        return;
    }
    if (wIsClipped)
    {
        rect.left   += pRect->left;
        rect.right  += pRect->right;
        rect.top    += pRect->top;
        rect.bottom += pRect->bottom;

        dwWidth = rect.right - rect.left + 1;
        dwHeight = rect.bottom - rect.top + 1;

        if (!(0x0002 & wIsClipped)) // TOP
        {
            pDest[0] = (DWORD*)(m_pLockedBackBuf + ((rect.top * m_dwLockedBackBufPitch) + (4 * rect.left)));
            for (DWORD i = 0; dwWidth > i; ++i)
            {
                *(pDest[0]) = (*(DWORD*)pEdge);
                pDest[0]++;
            }
        }
        if (!(0x0008 & wIsClipped)) // BOTTOM
        {
            pDest[0] = (DWORD*)(m_pLockedBackBuf + ((rect.bottom * m_dwLockedBackBufPitch) + (4 * rect.left)));
            for (DWORD i = 0; dwWidth > i; ++i)
            {
                *(pDest[0]) = (*(DWORD*)pEdge);
                pDest[0]++;
            }
        }
        if (!(0x0001 & wIsClipped)) // LEFT
        {
            pDest[0] = (DWORD*)(m_pLockedBackBuf + ((rect.top * m_dwLockedBackBufPitch) + (4 * rect.left)));
            for (DWORD i = 0; dwHeight > i; ++i)
            {
                *(pDest[0]) = (*(DWORD*)pEdge);
                (*(unsigned char**)(pDest)) += m_dwLockedBackBufPitch;
            }
        }
        if (!(0x0004 & wIsClipped)) // RIGHT
        {
            pDest[0] = (DWORD*)(m_pLockedBackBuf + ((rect.top * m_dwLockedBackBufPitch) + (4 * rect.right)));
            for (DWORD i = 0; dwHeight > i; ++i)
            {
                *(pDest[0]) = (*(DWORD*)pEdge);
                (*(unsigned char**)(pDest)) += m_dwLockedBackBufPitch;
            }
        }
    } else
    {
        dwWidth = pRect->right - pRect->left + 1;
        pDest[0] = (DWORD*)(m_pLockedBackBuf + ((pRect->top * m_dwLockedBackBufPitch) + (4 * pRect->left)));
        pDest[1] = (DWORD*)(m_pLockedBackBuf + ((pRect->bottom * m_dwLockedBackBufPitch) + (4 * pRect->left)));

        for (DWORD i = 0; dwWidth > i; ++i)
        {
            *(pDest[0]++) = (*(DWORD*)pEdge);
            *(pDest[1]++) = (*(DWORD*)pEdge);
        }

        dwHeight = pRect->bottom - pRect->top;
        pDest[0] = (DWORD*)(m_pLockedBackBuf + ((pRect->top * m_dwLockedBackBufPitch) + (4 * pRect->left)));
        pDest[1] = (DWORD*)(m_pLockedBackBuf + ((pRect->top * m_dwLockedBackBufPitch) + (4 * pRect->right)));

        for (DWORD i = 0; dwHeight > i; ++i)
        {
            *(pDest[0]) = (*(DWORD*)pEdge);
            *(pDest[1]) = (*(DWORD*)pEdge);
            (*(unsigned char**)(pDest)) += m_dwLockedBackBufPitch;
            (*(unsigned char**)(pDest + 1)) += m_dwLockedBackBufPitch;
        }
    }
}

void CDDrawRenderer::DrawRectFace(const RECT* pRect, const COLOR* pColor)
{
    RECT rect;
    DWORD dwWidth, dwHeight;
    DWORD* pDest[2];

#ifdef BEGINDRAW_FALSE_DEFEND
    if (!m_pLockedBackBuf)
    {
        return;
    }
#endif

    if (!QueryClippingRect(pRect, &m_rcMask, NULL, &rect))
    {
        return;
    }

    rect.top    += pRect->top;
    rect.left   += pRect->left;
    rect.right  += pRect->right;
    rect.bottom += pRect->bottom;

    dwWidth = rect.right - rect.left + 1;
    dwHeight = rect.bottom - rect.top + 1;

    // DWORD cd = (DWORD)pColor->B + (DWORD)pColor->B - (DWORD)pColor->G - (DWORD)pColor->R; // Color Difference
    DWORD cd;
    // if (cd)
    // {
    pDest[1] = (DWORD*)(m_pLockedBackBuf + ((rect.top * m_dwLockedBackBufPitch) + (4 * rect.left)));
    pDest[0] = (DWORD*)((*(unsigned char**)(pDest + 1)) + m_dwLockedBackBufPitch);

    for (DWORD i = 0; dwWidth > i; ++i)
    {
        pDest[1][i] = (*(DWORD*)pColor);
    }

    cd = 4 * dwWidth;
    for (DWORD y = 1; dwHeight > y; ++y)
    {
        memcpy(pDest[0], pDest[1], cd);
        (*(unsigned char**)(pDest)) += m_dwLockedBackBufPitch;
    }
    // } else
    // {
    //     pDest[0] = (DWORD*)(m_pLockedBackBuf + ((rect.top * m_dwLockedBackBufPitch) + (4 * rect.left)));
    //     cd = 4 * dwWidth;
    //     for (DWORD y = 0; dwHeight > y; ++y)
    //     {
    //         memset(pDest[0], pColor->B, cd);
    //         (*(unsigned char**)(pDest)) += m_dwLockedBackBufPitch;
    //     }
    // }
}

void CDDrawRenderer::DrawRect(const RECT* pRect, const COLOR* pEdge, const COLOR* pFace)
{
    if (pFace)
    {
        DrawRectFace(pRect, pFace);
    }
    if (pEdge)
    {
        DrawRectEdge(pRect, pEdge);
    }
}

void CDDrawRenderer::DrawBitmap(const POINT* pDest, const BITMAP_HANDLE* pBitmapHandle, const RECT* pClip)
{
    RECT rcRect;
    RECT rcDest;
    
    DWORD dwSize;
    DWORD dwLoop;
    DWORD dwPitch;

    unsigned char* pcsDest;
    const unsigned char* pcsSrc;

#ifdef BEGINDRAW_FALSE_DEFEND
    if (!m_pLockedBackBuf)
    {
        return;
    }
#endif

    rcDest.left     = pDest->x;
    rcDest.top      = pDest->y;
    rcDest.right    = pDest->x;
    rcDest.bottom   = pDest->y;

    if (pClip)
    {
        rcDest.right    += pClip->right - pClip->left - 1;
        rcDest.bottom   += pClip->bottom - pClip->top - 1;
    } else
    {
        rcDest.right    += pBitmapHandle->pCompressedBitmap->dwWidth - 1;
        rcDest.bottom   += pBitmapHandle->pCompressedBitmap->dwHeight - 1;
    }

    if (!QueryClippingRect(&rcDest, &m_rcMask, NULL, &rcRect))
    {
        return;
    }

    rcDest.left   += rcRect.left;
    rcDest.top    += rcRect.top;
    rcDest.right  += rcRect.right;
    rcDest.bottom += rcRect.bottom;

    if (pClip)
    {
        rcRect.left += pClip->left;
        rcRect.top  += pClip->top;
    }

    dwSize = (rcDest.right - rcDest.left + 1) * 4;
    dwLoop = rcDest.bottom - rcDest.top + 1;
    dwPitch = pBitmapHandle->pBitmap->dwWidth * 4;

    pcsDest = m_pLockedBackBuf + (m_dwLockedBackBufPitch * rcDest.top + rcDest.left * 4);
    pcsSrc = pBitmapHandle->pBitmap->pixels + (dwPitch * rcRect.top + rcRect.left * 4);

    for (DWORD i = 0; dwLoop > i; ++i)
    {
        memcpy(pcsDest, pcsSrc, dwSize);
        pcsDest += m_dwLockedBackBufPitch;
        pcsSrc  += dwPitch;
    }
}

void CDDrawRenderer::DrawBitmapWithColorKey(const POINT* pDest, const BITMAP_HANDLE* pBitmapHandle, const RECT* pClip, DWORD dwColorKey)
{
    RECT rcRect;
    RECT rcDest;

    DWORD dwSize;
    DWORD dwLoop;
    DWORD dwPitch;

    unsigned char* pcsDest;
    const unsigned char* pcsSrc;

#ifdef BEGINDRAW_FALSE_DEFEND
    if (!m_pLockedBackBuf)
    {
        return;
    }
#endif

    rcDest.left     = pDest->x;
    rcDest.top      = pDest->y;
    rcDest.right    = pDest->x;
    rcDest.bottom   = pDest->y;

    if (pClip)
    {
        rcDest.right    += pClip->right - pClip->left - 1;
        rcDest.bottom   += pClip->bottom - pClip->top - 1;
    } else
    {
        rcDest.right    += pBitmapHandle->pCompressedBitmap->dwWidth - 1;
        rcDest.bottom   += pBitmapHandle->pCompressedBitmap->dwHeight - 1;
    }

    if (!QueryClippingRect(&rcDest, &m_rcMask, NULL, &rcRect))
    {
        return;
    }

    rcDest.left     += rcRect.left;
    rcDest.top      += rcRect.top;
    rcDest.right    += rcRect.right;
    rcDest.bottom   += rcRect.bottom;

    if (pClip)
    {
        rcRect.left += pClip->left;
        rcRect.top  += pClip->top;
    }

    dwSize = (rcDest.right - rcDest.left + 1) * 4;
    dwLoop = rcDest.bottom - rcDest.top + 1;
    dwPitch = pBitmapHandle->pBitmap->dwWidth * 4;

    pcsDest = m_pLockedBackBuf + (m_dwLockedBackBufPitch * rcDest.top + rcDest.left * 4);
    pcsSrc = pBitmapHandle->pBitmap->pixels + (dwPitch * rcRect.top + rcRect.left * 4);

    DWORD dwColor;

    for (DWORD i = 0; dwLoop > i; ++i)
    {
        for (DWORD j = 0; dwSize > j; j += 4)
        {
            dwColor = *(DWORD*)(pcsSrc + j);
            if (dwColorKey != dwColor)
            {
                *(DWORD*)(pcsDest + j) = dwColor;
            }
        }
        pcsDest += m_dwLockedBackBufPitch;
        pcsSrc += dwPitch;
    }
}

void CDDrawRenderer::DrawCompressedBitmap(const POINT* pDest, const BITMAP_HANDLE* pBitmapHandle, const RECT* pClip)
{
    RECT rcRect;

    union {
        RECT rcDest;
        struct
        {
            DWORD   dwStreamNum;
            DWORD   dwColor;
            int     iDestX;
            int     iPixelNum;
        };
    };
    

#ifdef BEGINDRAW_FALSE_DEFEND
    if (!m_pLockedBackBuf)
    {
        return;
    }
#endif

#ifdef _DEBUG
    if (!pBitmapHandle->pCompressedBitmap)
    {
        __debugbreak();
    }
#endif // _DEBUG

    rcDest.left     = pDest->x;
    rcDest.top      = pDest->y;
    rcDest.right    = pDest->x;
    rcDest.bottom   = pDest->y;

    if (pClip)
    {
        rcDest.right    += pClip->right - pClip->left - 1;
        rcDest.bottom   += pClip->bottom - pClip->top - 1;
    } else
    {
        rcDest.right    += pBitmapHandle->pCompressedBitmap->dwWidth - 1;
        rcDest.bottom   += pBitmapHandle->pCompressedBitmap->dwHeight - 1;
    }

    if (!QueryClippingRect(&rcDest, &m_rcMask, NULL, &rcRect))
    {
        return;
    }

    rcDest.left     += rcRect.left;
    rcDest.top      += rcRect.top;
    rcDest.right    += rcRect.right;
    rcDest.bottom   += rcRect.bottom;

    if (pClip)
    {
        rcRect.left += pClip->left;
        rcRect.top += pClip->top;
    }

    COMPRESSED_PIXEL_LINE* pCPL = pBitmapHandle->pCompressedBitmap->pStreamLine + rcRect.top;
    BYTE*   pDestPerLine    = m_pLockedBackBuf + (m_dwLockedBackBufPitch * rcDest.top + rcDest.left * 4);
    DWORD   dwLoop          = rcDest.bottom - rcDest.top + 1;
    LONG    iEnd            = rcDest.right - rcDest.left + 1;

    dwStreamNum = 0;
    dwColor     = 0;
    iDestX      = 0;
    iPixelNum   = 0;

    for (DWORD y = 0; dwLoop > y; ++y)
    {
        PIXEL_STREAM* pPS = pCPL->pPixelStream;
        BYTE* pcsDest   = pDestPerLine;
        dwStreamNum     = pCPL->dwStreamNum;

        for (DWORD i = 0; dwStreamNum > i; ++i)
        {
            iDestX = (int)pPS->wPosX - rcRect.left;

            if (iDestX > iEnd)
            {
                goto lb_next;
            }

            iPixelNum   = pPS->wPixelNum;
            dwColor     = pPS->dwColor;

            if (0 > iDestX)
            {
                iPixelNum += iDestX;
                iDestX = 0;
            }

            if (iDestX + iPixelNum > iEnd)
            {
                iPixelNum = iEnd - iDestX;
            }       

            pcsDest = pDestPerLine + (iDestX * 4);

            for (int x = 0; iPixelNum > x; ++x)
            {
                *(DWORD*)pcsDest = dwColor;
                pcsDest += 4;
            }
            pPS++;
        }
lb_next:
        pCPL++;
        pDestPerLine += m_dwLockedBackBufPitch;
    }
}

void CDDrawRenderer::GetWindowRectAndSize(RECT* prcRect, DWORD* pdwWidth, DWORD* pdwHeight)
{
    if (prcRect)
    {
        (*prcRect) = m_rcWindow;
    }
    if (pdwWidth)
    {
        (*pdwWidth) = m_dwWidth;
    }
    if (pdwHeight)
    {
        (*pdwHeight) = m_dwHeight;
    }
}

void* CDDrawRenderer::CreateSpriteObject(void)
{
    CSpriteObject* pSprite = new CSpriteObject;
    pSprite->Initialize(this);
    return pSprite;
}

void CDDrawRenderer::DeleteSpriteObject(void* pSpriteObject)
{
    CSpriteObject* pSprite = (CSpriteObject*)pSpriteObject;
    delete pSprite;
}

BOOL CDDrawRenderer::BeginCreateSpriteObject(void* pSpriteObject, DWORD MaxAnimationNum)
{
    CSpriteObject* pSprite = (CSpriteObject*)pSpriteObject;
    BOOL bRsl = pSprite->BeginCreateSprite(MaxAnimationNum);
    return bRsl;
}

BOOL CDDrawRenderer::CreateSpriteDefaultBitmap(void* pSpriteObject, const WCHAR* wcsFileName, const RECT* prcClip, const COLOR* pColorKey)
{
    CSpriteObject* pSprite = (CSpriteObject*)pSpriteObject;
    BOOL bRsl = pSprite->CreateDefaultBitmap(wcsFileName, prcClip, pColorKey);
    return bRsl;
}

BOOL CDDrawRenderer::InsertSpriteAnimation(void* pSpriteObject, const WCHAR* wcsFileName, const POINT* pClipPos, DWORD dwClipWidth, DWORD dwClipHeight, const COLOR* pColorKey, DWORD dwFrameNum, DWORD dwPitch, float fFps, BOOL bRepeat)
{
    CSpriteObject* pSprite = (CSpriteObject*)pSpriteObject;
    RECT rcClip;
    rcClip.left = pClipPos->x;
    rcClip.top  = pClipPos->y;
    rcClip.right    = rcClip.left + dwClipWidth;
    rcClip.bottom   = rcClip.top  + dwClipHeight;
    BOOL bRsl = pSprite->InsertAnimation(wcsFileName, &rcClip, pColorKey, dwFrameNum, dwPitch, fFps, bRepeat);
    return bRsl;
}

void CDDrawRenderer::EndCreateSpriteObject(void* pSpriteObject)
{
    CSpriteObject* pSprite = (CSpriteObject*)pSpriteObject;
    pSprite->EndCreateSprite();
}

void CDDrawRenderer::SetSpriteAnimation(void* pSpriteObject, DWORD dwAnimationIndex, DWORD dwFrame)
{
    CSpriteObject* pSprite = (CSpriteObject*)pSpriteObject;
    pSprite->SetAnimationNum(dwAnimationIndex, dwFrame);
}

void CDDrawRenderer::PlaySpriteAnimation(void* pSpriteObject, ULONGLONG CurTick)
{
    CSpriteObject* pSprite = (CSpriteObject*)pSpriteObject;
    pSprite->Play(CurTick);
}

void CDDrawRenderer::RenderSpriteObject(void* pSpriteObject, const POINT* pPos)
{
    CSpriteObject* pSprite = (CSpriteObject*)pSpriteObject;
    pSprite->Draw(pPos);
}

void CDDrawRenderer::Cleanup(void)
{
    if (m_pBitmapManager)
    {
        delete m_pBitmapManager;
        m_pBitmapManager = nullptr;
    }
    if (m_pDDrawClipper)
    {
        m_pDDrawClipper->Release();
        m_pDDrawClipper = nullptr;
    }
    if (m_pDDrawSub)
    {
        m_pDDrawSub->Release();
        m_pDDrawSub = nullptr;
    }
    if (m_pDDrawBack)
    {
        m_pDDrawBack->Release();
        m_pDDrawBack = nullptr;
    }
    if (m_pDDrawFront)
    {
        m_pDDrawFront->Release();
        m_pDDrawFront = nullptr;
    }
    if (m_pDDraw7)
    {
        m_pDDraw7->Release();
        m_pDDraw7 = nullptr;
    }
    if (m_pDDraw)
    {
        m_pDDraw->Release();
        m_pDDraw = nullptr;
    }
}

HRESULT CDDrawRenderer::CreateBackBuffer(WORD width, WORD height)
{
    HRESULT hr;
    DDSURFACEDESC2 ddsd;

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize     = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags    = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth    = width;
    ddsd.dwHeight   = height;

    hr = m_pDDraw7->CreateSurface(&ddsd, &m_pDDrawBack, NULL);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    if (!m_pDDrawSub)
    {
        hr = m_pDDrawBack->GetSurfaceDesc(&ddsd);
        if (FAILED(hr))
        {
            goto lb_return;
        }

        m_dwWidth = ddsd.dwWidth;
        m_dwHeight = ddsd.dwHeight;
        m_rcMask.right = m_dwWidth - 1;
        m_rcMask.bottom = m_dwHeight - 1;
    }

lb_return:
    return hr;
}

void CDDrawRenderer::DeleteBackBuffer(void)
{
    if (m_pDDrawBack)
    {
        m_pDDrawBack->Release();
        m_pDDrawBack = NULL;
    }
}

BOOL CDDrawRenderer::LockBackBuffer(unsigned char** ppPixels, DWORD* pdwW, DWORD* pdwH, DWORD* pdwPitch)
{
    HRESULT hr;
    DDSURFACEDESC2 ddsc;
    if (m_pDDrawBack)
    {
        ZeroMemory(&ddsc, sizeof(ddsc));
        ddsc.dwSize = sizeof(DDSURFACEDESC2);
        if (m_pDDrawSub)
        {
            hr = m_pDDrawSub->Lock(NULL, &ddsc, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
        } else
        {
            hr = m_pDDrawBack->Lock(NULL, &ddsc, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
        }
        
        if (SUCCEEDED(hr))
        {
            (*ppPixels) = (unsigned char*)ddsc.lpSurface;
            (*pdwW) = ddsc.dwWidth;
            (*pdwH) = ddsc.dwHeight;
            (*pdwPitch) = ddsc.lPitch;
            return TRUE;
        }
    } return FALSE;
}

void CDDrawRenderer::UnlockBackBuffer(void)
{
    if (m_pDDrawSub)
    {
        m_pDDrawSub->Unlock(NULL);
    } else
    {
        m_pDDrawBack->Unlock(NULL);
    }
}

BOOL CDDrawRenderer::QueryPointInRect(const POINT* pPoint, const RECT* pMask) const
{
    BOOL bInner = FALSE;
    if (pMask->left > pPoint->x)
    {
        goto lb_exit;
    }
    if (pMask->right <= pPoint->x)
    {
        goto lb_exit;
    }
    if (pMask->top > pPoint->y)
    {
        goto lb_exit;
    }
    if (pMask->bottom <= pPoint->y)
    {
        goto lb_exit;
    }
    bInner = TRUE;

lb_exit:
    return bInner;
}

BOOL CDDrawRenderer::QueryClippingRect(const RECT* pTarget, const RECT* pMask, WORD* pwIsClipped, RECT* pOffset) const
{
    RECT offset = {};
    BOOL bInner = FALSE;
    WORD bIsClipped = 0;

    if (pMask->right <= pTarget->left)
    {
        goto lb_return;
    }
    if (pMask->bottom <= pTarget->top)
    {
        goto lb_return;
    }
    if (pMask->left > pTarget->right)
    {
        goto lb_return;
    }
    if (pMask->top > pTarget->bottom)
    {
        goto lb_return;
    }

    if (pMask->left > pTarget->left)
    {
        offset.left = pMask->left - pTarget->left;
        bIsClipped += 0x0001;
    }
    if (pMask->top > pTarget->top)
    {
        offset.top = pMask->top - pTarget->top;
        bIsClipped += 0x0002;
    }
    if (pMask->right < pTarget->right)
    {
        offset.right = pMask->right - pTarget->right;
        bIsClipped += 0x0004;
    }
    if (pMask->bottom < pTarget->bottom)
    {
        offset.bottom = pMask->bottom - pTarget->bottom;
        bIsClipped += 0x0008;
    }

    if (pwIsClipped)
    {
        (*pwIsClipped) = bIsClipped;
    }
    if (pOffset)
    {
        (*pOffset) = offset;
    }
    bInner = TRUE;

lb_return:
    return bInner;
}