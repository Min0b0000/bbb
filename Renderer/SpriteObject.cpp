#include "SpriteObject.h"
#include "DDrawRenderer.h"
#include "BitmapManager.h"



CSpriteObject::CSpriteObject(void)
{
}

CSpriteObject::~CSpriteObject(void)
{
	Cleanup();
}

BOOL CSpriteObject::Initialize(CDDrawRenderer* pRenderer)
{
	m_pRenderer = pRenderer;
	return TRUE;
}

BOOL CSpriteObject::BeginCreateSprite(DWORD MaxAnimationNum)
{
	BOOL bRsl = FALSE;
	DWORD pSize = sizeof(SPRITE_ANIMATION_DESC) * MaxAnimationNum;

	if (!pSize)
	{
		goto lb_return;
	}

	m_pSpriteAnimationDesc = (SPRITE_ANIMATION_DESC*)malloc(pSize);
	memset(m_pSpriteAnimationDesc, 0, pSize);

	m_dwMaxAnimationNum = MaxAnimationNum;

	bRsl = TRUE;

lb_return:
	return bRsl;
}

BOOL CSpriteObject::CreateDefaultBitmap(const WCHAR* wcsFileName, const RECT* prcClip, const COLOR* pColorKey)
{
	BOOL bRsl = FALSE;
	CBitmapManager* pBitmapManager = m_pRenderer->INL_GetBitmapManager();

	m_pDefaultBitmapHandle = pBitmapManager->CreateBitmapFromBMPFile(wcsFileName);

	if (!m_pDefaultBitmapHandle)
	{
		goto lb_return;
	}

	if (pColorKey) // TODO
	{
		pBitmapManager->CreateCompressBitmap((BITMAP_HANDLE*)m_pDefaultBitmapHandle, pColorKey);
	}

	if (prcClip)
	{
		m_rcDefaultClip = (*prcClip);
	} else
	{
		BITMAP_HANDLE* pBit = (BITMAP_HANDLE*)m_pDefaultBitmapHandle;		
		m_rcDefaultClip = { };
		m_rcDefaultClip.right	= pBit->pBitmap->dwWidth;
		m_rcDefaultClip.bottom = pBit->pBitmap->dwHeight;
	}
	
	bRsl = TRUE;

lb_return:
	return bRsl;
}

BOOL CSpriteObject::InsertAnimation(const WCHAR* wcsFileName, const RECT* prcClip, const COLOR* pColorKey, DWORD dwFrameNum, DWORD dwPitch, ULONGLONG uDelay, BOOL bRepeat)
{
	BOOL bRsl = FALSE;
	CBitmapManager* pBitmapManager = m_pRenderer->INL_GetBitmapManager();
	SPRITE_ANIMATION_DESC* pSad; // Are you sad? =w=(lol
	
	if (m_dwMaxAnimationNum > m_dwHasAnimationNum)
	{
		pSad = m_pSpriteAnimationDesc + m_dwHasAnimationNum;
		m_dwHasAnimationNum++;
	} else
	{
		__debugbreak();
		goto lb_return;
	}

	memset(pSad, 0, sizeof(SPRITE_ANIMATION_DESC));
	
	pSad->pBitmapHandle = pBitmapManager->CreateBitmapFromBMPFile(wcsFileName);
	if (!pSad->pBitmapHandle)
	{
		__debugbreak();
		goto lb_return;
	}

	if (pColorKey) // TODO
	{
		pBitmapManager->CreateCompressBitmap((BITMAP_HANDLE*)pSad->pBitmapHandle, pColorKey);
		pSad->m_bIsCompressed = TRUE;
	}

	if (!dwPitch)
	{
		dwPitch = dwFrameNum;
	}

	pSad->rcClip		= (*prcClip);
	pSad->dwFrameNum	= dwFrameNum;
	pSad->dwFramePitch	= dwPitch;
	pSad->uDelay		= uDelay;
	pSad->uPrvTick		= 0;
	pSad->bDoRepeat		= bRepeat;

	// bRsl;
	// if (!bRsl)
	// {
	// 	pBitmapManager->DeleteBitmap((BITMAP_HANDLE*)pSad->pBitmapHandle);
	// 	__debugbreak();
	// }

lb_return:
	return bRsl;
}

void CSpriteObject::EndCreateSprite(void)
{
	if (m_dwMaxAnimationNum != m_dwHasAnimationNum)
	{
		__debugbreak();
	}
}

void CSpriteObject::SetAnimationNum(DWORD dwAnimationIndex, DWORD dwFrame)
{
	SPRITE_ANIMATION_DESC* pSad = NULL;
	
	if (DW_SPRITE_DEFAULT == dwAnimationIndex)
	{
		m_dwCurAnimationIndex = dwAnimationIndex;
		return;
	}

	m_dwCurAnimationIndex = dwAnimationIndex;
	if (m_dwMaxAnimationNum <= dwAnimationIndex)
	{
		__debugbreak();
	}
	pSad = m_pSpriteAnimationDesc + dwAnimationIndex;
	if (pSad->dwFrameNum <= dwFrame)
	{
		__debugbreak();
	}
	pSad->uPrvTick = 0;
	pSad->dwNowFrame = dwFrame;
}

void CSpriteObject::Play(ULONGLONG CurTick)
{
	if (m_dwMaxAnimationNum <= m_dwCurAnimationIndex)
	{
		if (DW_SPRITE_DEFAULT != m_dwCurAnimationIndex)
		{
			__debugbreak();
		}

		m_pBitmapHandle = m_pDefaultBitmapHandle;
		m_rcClip = m_rcDefaultClip;
		m_bIsUpdatedFrame = FALSE;
		m_bIsLastFrame = FALSE;
		return;
	}

	SPRITE_ANIMATION_DESC* pSad = m_pSpriteAnimationDesc + m_dwCurAnimationIndex;
	m_bIsCompressed = pSad->m_bIsCompressed;
	m_bIsUpdatedFrame = FALSE;

	if (CurTick - pSad->uDelay >= pSad->uPrvTick)
	{
		DWORD dwNow = pSad->dwNowFrame;
		dwNow += (pSad->uPrvTick ? 1 : 0);

		if (pSad->dwFrameNum <= dwNow)
		{
			dwNow = (pSad->bDoRepeat ? 0 : pSad->dwNowFrame);
			m_bIsLastFrame = TRUE;
		} else
		{
			m_bIsLastFrame = FALSE;
		}

		pSad->uPrvTick = CurTick;
		UpdateFrame(pSad, dwNow, &m_pBitmapHandle, &m_rcClip);
	}
}

void CSpriteObject::Draw(const POINT* pPos)
{
#ifdef _DEBUG
	if (!m_pBitmapHandle)
	{
		__debugbreak();
	}
#endif // _DEBUG

	if (m_bIsCompressed)
	{
		m_pRenderer->DrawCompressedBitmap(pPos, (BITMAP_HANDLE*)m_pBitmapHandle, &m_rcClip);
		// m_pRenderer->DrawBitmapWithColorKey(pPos, (BITMAP_HANDLE*)m_pBitmapHandle, &m_rcClip, 0xffff00ff);
	} else
	{
		m_pRenderer->DrawBitmap(pPos, (BITMAP_HANDLE*)m_pBitmapHandle, &m_rcClip);
	}
}

void CSpriteObject::Cleanup(void)
{
	SPRITE_ANIMATION_DESC* pSad = m_pSpriteAnimationDesc;
	CBitmapManager* pBitmapManager = m_pRenderer->INL_GetBitmapManager();
	pBitmapManager->DeleteBitmap((BITMAP_HANDLE*)m_pDefaultBitmapHandle);
	for (DWORD i = 0; m_dwMaxAnimationNum > i; ++i)
	{
		pBitmapManager->DeleteBitmap((BITMAP_HANDLE*)pSad->pBitmapHandle);
		pSad++;
	}
	free(m_pSpriteAnimationDesc);
}

BOOL CSpriteObject::UpdateFrame(SPRITE_ANIMATION_DESC* pSad, DWORD dwNum, void** pBitmap, RECT* pClip)
{
	BOOL bRsl = FALSE;
	m_bIsUpdatedFrame = TRUE;
	
	DWORD dwWidth, dwHeight;
	dwWidth		= pSad->rcClip.right - pSad->rcClip.left;
	dwHeight	= pSad->rcClip.bottom - pSad->rcClip.top;

	(*pBitmap)		= pSad->pBitmapHandle;

	(*pClip).left	= pSad->rcClip.left	+ dwWidth	* (dwNum % pSad->dwFramePitch);
	(*pClip).top	= pSad->rcClip.top	+ dwHeight	* (dwNum / pSad->dwFramePitch);
	(*pClip).right	= (*pClip).left + dwWidth;
	(*pClip).bottom = (*pClip).top	+ dwHeight;

	pSad->dwNowFrame = dwNum;
	
	bRsl = TRUE;

lb_return:
	return bRsl;
}
