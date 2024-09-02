#pragma once

#include "../framework.h"
#include "../Typedef.h"

const DWORD DW_SPRITE_DEFAULT = -1;

struct SPRITE_ANIMATION_DESC
{
	void*	pBitmapHandle;
	BOOL	m_bIsCompressed = FALSE;
	RECT	rcClip;
	DWORD	dwFrameNum;
	DWORD	dwFramePitch;
	DWORD	dwNowFrame;
	ULONGLONG uDelay;
	ULONGLONG uPrvTick;
	BOOL	bDoRepeat;
};



class CDDrawRenderer;

class CSpriteObject
{
public:
	CSpriteObject();
	~CSpriteObject();

	BOOL Initialize(CDDrawRenderer* pRenderer);

	BOOL BeginCreateSprite(DWORD MaxAnimationNum);
	BOOL CreateDefaultBitmap(const WCHAR* wcsFileName, const RECT* prcClip, const COLOR* pColorKey);
	BOOL InsertAnimation(const WCHAR* wcsFileName, const RECT* prcClip, const COLOR* pColorKey, DWORD dwFrameNum, DWORD dwPitch, ULONGLONG uDelay, BOOL bRepeat);
	void EndCreateSprite();

	void SetAnimationNum(DWORD dwAnimationIndex, DWORD dwFrame);
	DWORD GetAnimationNum() const { return m_dwCurAnimationIndex; };
	void Play(ULONGLONG CurTicks);
	void Draw(const POINT* pPos);

	BOOL IsUpdatedFrame() const { return m_bIsUpdatedFrame; }
	BOOL IsLastFrame() const { return m_bIsLastFrame; }

private:
	void Cleanup();

	BOOL UpdateFrame(SPRITE_ANIMATION_DESC* pSad, DWORD dwNum, void** pBitmap, RECT* pClip);

private:
	CDDrawRenderer*			m_pRenderer				= nullptr;
	SPRITE_ANIMATION_DESC*	m_pSpriteAnimationDesc	= nullptr;
	DWORD	m_dwMaxAnimationNum		= 0;
	DWORD	m_dwHasAnimationNum		= 0;
	DWORD	m_dwCurAnimationIndex	= 0;
	BOOL	m_bIsUpdatedFrame	= FALSE;
	BOOL	m_bIsLastFrame		= FALSE;
	BOOL	m_bIsCompressed		= FALSE;
	void*	m_pDefaultBitmapHandle = nullptr;
	RECT	m_rcDefaultClip		= {};

	void*	m_pBitmapHandle		= nullptr;
	RECT	m_rcClip			= {};
};