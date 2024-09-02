#pragma once

#include "../framework.h"
#include "../Typedef.h"



// #define BEGINDRAW_FALSE_DEFEND

class IDirectDraw;
class IDirectDraw7;
class IDirectDrawSurface7;
class IDirectDrawClipper;
class CBitmapManager;

class CDDrawRenderer
{
public:
	static const DWORD BITMAPMANAGER_MAX_BUCKUT_NUM = 256;

public:
	CDDrawRenderer(void);
	~CDDrawRenderer(void);

	HRESULT	Initialize(HWND hWnd);

	void UpdateWindowPos(void);
	void UpdateWindowSize(void);

	HRESULT CreateSubBuffer(WORD width, WORD height);
	void    DeleteSubBuffer(void);

	BOOL BeginDraw(void);
	void EndDraw(void);
	
	void DrawClear(const COLOR* pColor);
	
	inline void DrawDot(DWORD x, DWORD y, const COLOR* pColor); /**< non-safe */
	void DrawPixel(DWORD x, DWORD y, const COLOR* pColor);

	void DrawRectEdge(const RECT* pRect, const COLOR* pColor);
	void DrawRectFace(const RECT* pRect, const COLOR* pColor);
	void DrawRect(const RECT* pRect, const COLOR* pEdge, const COLOR* pFace);
	
	void DrawBitmap(const POINT* pDest, const BITMAP_HANDLE* pBitmapHandle, const RECT* pClip);
	void DrawBitmapWithColorKey(const POINT* pDest, const BITMAP_HANDLE* pBitmapHandle, const RECT* pClip, DWORD dwColorKey);
	void DrawCompressedBitmap(const POINT* pDest, const BITMAP_HANDLE* pBitmapHandle, const RECT* pClip);

	void GetWindowRectAndSize(RECT* prcRect, DWORD* pdwWidth, DWORD* pdwHeight);

	void* CreateSpriteObject(void);
	void DeleteSpriteObject(void* pSpriteObject);
	BOOL BeginCreateSpriteObject(void* pSpriteObject, DWORD MaxAnimationNum);
	BOOL CreateSpriteDefaultBitmap(void* pSpriteObject, const WCHAR* wcsFileName, const RECT* prcClip, const COLOR* pColorKey);
	BOOL InsertSpriteAnimation(void* pSpriteObject, const WCHAR* wcsFileName, const POINT* pClipPos, DWORD dwClipWidth, DWORD dwClipHeight, const COLOR* pColorKey, DWORD dwFrameNum, DWORD dwPitch, float fFps, BOOL bRepeat);
	void EndCreateSpriteObject(void* pSpriteObject);

	void SetSpriteAnimation(void* pSpriteObject, DWORD dwAnimationIndex, DWORD dwFrame);
	void PlaySpriteAnimation(void* pSpriteObject, ULONGLONG CurTick);
	void RenderSpriteObject(void* pSpriteObject, const POINT* pPos);

	HWND			INL_GetHWND(void) const { return m_hWnd; };
	IDirectDraw*	INL_GetIDirectDraw(void) const { return m_pDDraw; };
	IDirectDraw7*	INL_GetIDirectDraw7(void) const { return m_pDDraw7; };
	CBitmapManager*	INL_GetBitmapManager(void) const { return m_pBitmapManager; };

private:
	void Cleanup(void);

	HRESULT CreateBackBuffer(WORD width, WORD height);
	void    DeleteBackBuffer(void);

	BOOL LockBackBuffer(unsigned char** ppPixels, DWORD* pdwW, DWORD* pdwH, DWORD* pdwPitch);
	void UnlockBackBuffer(void);

	BOOL QueryPointInRect(const POINT* pPoint, const RECT* pMask) const;
	BOOL QueryClippingRect(const RECT* pTarget, const RECT* pMask, WORD* pwIsClipped, RECT* pOffset) const; /**< wIsClipped<bit_flag> = { left:1, top:2, right:4, bottom:8 }*/

private:
	HWND					m_hWnd = NULL;
	IDirectDraw*			m_pDDraw = nullptr;
	IDirectDraw7*			m_pDDraw7 = nullptr;
	IDirectDrawSurface7*	m_pDDrawFront = nullptr;
	IDirectDrawSurface7*	m_pDDrawBack = nullptr;
	IDirectDrawSurface7*	m_pDDrawSub = nullptr;
	IDirectDrawClipper*		m_pDDrawClipper = nullptr;
	CBitmapManager*			m_pBitmapManager = nullptr;
	unsigned char*			m_pLockedBackBuf	= nullptr;
	DWORD					m_dwLockedBackBufPitch = 0;
	RECT					m_rcWindow = {};
	DWORD					m_dwWidth = 0;
	DWORD					m_dwHeight = 0;
	RECT					m_rcMask = {};
};

inline void CDDrawRenderer::DrawDot(DWORD x, DWORD y, const COLOR* pColor)
{
	(*(DWORD*)(m_pLockedBackBuf + ((y * m_dwLockedBackBufPitch) + (4 * x)))) = (*(DWORD*)pColor);
}