#include "Game.h"
#include "../Renderer/DDrawRenderer.h"
#include "GameObject.h"
#include "UI.h"
#include "TileMap.h"
#include "FnGameObjectUpdate.h"



CGame::CGame(void)
{

}

CGame::~CGame(void)
{
	Cleanup();
}

BOOL CGame::Initialize(HWND hWnd)
{
	HRESULT hr;
	BOOL bRsl = FALSE;
	
	m_hWnd = hWnd;
	
	m_pDDrawRenderer = new CDDrawRenderer;
	hr = m_pDDrawRenderer->Initialize(m_hWnd);
	if (FAILED(hr))
	{
		goto lb_return;
	}

	bRsl = TRUE;

lb_return:
	return bRsl;
}

void CGame::Run(void)
{
	m_CurTick = GetTickCount64();
	Update(m_CurTick);

	Render();

	m_dwFrameCount++;
	if (m_CurTick - m_PrvFrameCheckTick > 1000)
	{
		m_PrvFrameCheckTick = m_CurTick;
		m_dwFPS = m_dwFrameCount;
		m_dwFrameCount = 0;

#ifdef _DEBUG
		WCHAR wchTxt[256];
		swprintf_s(wchTxt, L"FPS : %u, GameObj : %u, RndObj : %u", m_dwFPS, m_dwGameObjNum, m_dwRndObjNum);
		SetWindowText(m_hWnd, wchTxt);
#endif
	}
}

void CGame::CreateVirtualResolution(WORD width, WORD height)
{
	m_pDDrawRenderer->CreateSubBuffer(width, height);
}

void CGame::DeleteVirtualResolution(void)
{
	m_pDDrawRenderer->DeleteSubBuffer();
}

void CGame::UpdateWindowPos(void)
{
	m_pDDrawRenderer->UpdateWindowPos();
}

void CGame::UpdateWindowSize(void)
{
	m_pDDrawRenderer->UpdateWindowSize();
}

void CGame::SetCamPosition(float fX, float fY, float fZ)
{
	m_f3CamPosition = { fX, fY, fZ };
	// m_pDDrawRenderer->UpdateCamPosition;
}

void CGame::InsertRenderList(CGameObject* pGameObj, float fDepth)
{
	m_dwRndObjNum++;
	if (1.f == fDepth)
	{
		LinkToLinkedListFIFO(&m_pRenderSortLinkHead, &m_pRenderSortLinkTail, &pGameObj->onRenderLinkNode);
		return;
	}

	if (0.f == fDepth)
	{
		LinkToLinkedList(&m_pRenderSortLinkHead, &m_pRenderSortLinkTail, &pGameObj->onRenderLinkNode);
		return;
	}

	float fCurDepth;
	LINK_NODE* pIter;
	CGameObject* pCurObj;

	if (!m_pRenderSortLinkHead)
	{
		LinkToLinkedList(&m_pRenderSortLinkHead, &m_pRenderSortLinkTail, &pGameObj->onRenderLinkNode);
		return;
	}

	if (0.5f < fDepth)
	{
		pIter = m_pRenderSortLinkHead;
		while (pIter)
		{
			pCurObj = (CGameObject*)pIter->pData;
			fCurDepth = pCurObj->GetDepth();
			if (fCurDepth <= fDepth)
			{
				pGameObj->onRenderLinkNode.pNext = pIter;
				pGameObj->onRenderLinkNode.pPrev = pIter->pPrev;
				if (!pIter->pPrev)
				{
					m_pRenderSortLinkHead = &pGameObj->onRenderLinkNode;
				}
				pIter->pPrev = &pGameObj->onRenderLinkNode;
				return;
			}
			pIter = pIter->pNext;
		} LinkToLinkedList(&m_pRenderSortLinkHead, &m_pRenderSortLinkTail, &pGameObj->onRenderLinkNode);
	} else
	{
		pIter = m_pRenderSortLinkTail;
		while (pIter)
		{
			pCurObj = (CGameObject*)pIter->pData;
			fCurDepth = pCurObj->GetDepth();
			if (fCurDepth >= fDepth)
			{
				pGameObj->onRenderLinkNode.pPrev = pIter;
				pGameObj->onRenderLinkNode.pNext = pIter->pNext;
				if (!pIter->pNext)
				{
					m_pRenderSortLinkTail = &pGameObj->onRenderLinkNode;
				}
				pIter->pNext = &pGameObj->onRenderLinkNode;
				return;
			}
			pIter = pIter->pPrev;
		} LinkToLinkedListFIFO(&m_pRenderSortLinkHead, &m_pRenderSortLinkTail, &pGameObj->onRenderLinkNode);
	}
}

void CGame::CreatePlayer(float x, float y)
{
	CGameObject* pPlayer = CreateGameObject();

	void* pSprite = pSprite = m_pDDrawRenderer->CreateSpriteObject();

	COLOR	colorKey = { 0xff, 0x00, 0xff, 0xff };
	POINT	pos = {};

	m_pDDrawRenderer->BeginCreateSpriteObject(pSprite, 2);
	m_pDDrawRenderer->CreateSpriteDefaultBitmap(pSprite, L"Resource\\Player.bmp", NULL, NULL);

	pos = { 0, 0 };
	m_pDDrawRenderer->InsertSpriteAnimation(pSprite, L"Resource\\Player.bmp", &pos, 32, 32, &colorKey, 4, 4, 100, TRUE);

	pos = { 0, 32 };
	m_pDDrawRenderer->InsertSpriteAnimation(pSprite, L"Resource\\Player.bmp", &pos, 32, 32, &colorKey, 6, 4, 100, TRUE);

	m_pDDrawRenderer->EndCreateSpriteObject(pSprite);
	m_pDDrawRenderer->SetSpriteAnimation(pSprite, 0, 0);

	pPlayer->InsertActor(FnPlayerUpdate, pSprite, GAMEOBJ_RENDER_SPRITE);
	pPlayer->SetWorldPosition(x, y, 0.0f);
}

void CGame::OnMouseMove(WPARAM wParam, LPARAM lParam)
{

}

void CGame::OnMouseWheel(UINT message, WPARAM wParam, LPARAM lParam)
{

}

void CGame::OnMouseButtonDown(UINT message, WPARAM wParam, LPARAM lParam)
{

}

void CGame::OnMouseButtonUp(UINT message, WPARAM wParam, LPARAM lParam)
{

}

void CGame::OnKeyDown(UINT message, WPARAM wParam, LPARAM lParam)
{
	
}

void CGame::OnKeyUp(UINT message, WPARAM wParam, LPARAM lParam)
{

}

void CGame::Cleanup(void)
{
	DeleteAllGameObjects();

	if (m_pDDrawRenderer)
	{
		delete m_pDDrawRenderer;
		m_pDDrawRenderer = nullptr;
	}
}

BOOL CGame::Update(ULONGLONG CurTick)
{
	if (CurTick - m_PrvUpdateTick < U_GAME_UPDATELATE)
	{
		return FALSE;
	}

	m_PrvUpdateTick = CurTick;

	LINK_NODE* pIter = NULL;
	pIter = m_pGameObjLinkHead;
	while (pIter)
	{
		CGameObject* pGameObject = (CGameObject*)pIter->pData;
		pGameObject->Run();
		pIter = pIter->pNext;
	}

	return TRUE;
}

void CGame::Render(void)
{
	const COLOR BackGroundColor = { 0xa3, 0xa3, 0xa3, 0xa3 };
	if (!m_pDDrawRenderer->BeginDraw())
	{
		return;
	}
	m_pDDrawRenderer->DrawClear(&BackGroundColor);

	m_dwRndObjNum = 0;

	LINK_NODE* pIter = NULL;
	pIter = m_pGameObjLinkHead;
	while (pIter)
	{
		CGameObject* pGameObject = (CGameObject*)pIter->pData;
		InsertRenderList(pGameObject, pGameObject->GetDepth());
		pIter = pIter->pNext;
	}

	DWORD dwWidthD2, dwHeightD2;
	m_pDDrawRenderer->GetWindowRectAndSize(NULL, &dwWidthD2, &dwHeightD2);
	dwWidthD2 >>= 1;
	dwHeightD2 >>= 1;

	pIter = m_pRenderSortLinkHead;
	while (pIter)
	{
		CGameObject* pGameObject = (CGameObject*)pIter->pData;
		GAMEOBJECT_RENDER_TYPE GameObjRenderType;
		void* pRenderObject = pGameObject->INL_GetRenderObject(&GameObjRenderType);

#ifdef _DEBUG
		if (GameObjRenderType)
		{
			if (!pRenderObject)
			{
				__debugbreak();
			}
		}
#endif
		switch (GameObjRenderType)
		{
		case GAMEOBJ_RENDER_UNKNOWN:
			break;
		case GAMEOBJ_RENDER_SPRITE:
		{
			POINT Pos;
			const float3* pWorld = pGameObject->GetWorldPosition();
			Pos.x = (LONG)pWorld->x - m_f3CamPosition.x + dwWidthD2;
			Pos.y = -(LONG)pWorld->y + m_f3CamPosition.y + dwHeightD2;
			m_pDDrawRenderer->RenderSpriteObject(pRenderObject, &Pos);
		} break;
		case GAMEOBJ_RENDER_TILEMAP:
			break;
		default:
			break;
		}
		pIter = pIter->pNext;
	}

	m_pRenderSortLinkHead = nullptr;
	m_pRenderSortLinkTail = nullptr;

	m_pDDrawRenderer->EndDraw();
}

CGameObject* CGame::CreateGameObject()
{
	CGameObject* pGameObj = new CGameObject;
	pGameObj->Initialize(this);
	LinkToLinkedListFIFO(&m_pGameObjLinkHead, &m_pGameObjLinkTail, &pGameObj->inGameLinkNode);
	m_dwGameObjNum++;
	return pGameObj;
}

void CGame::DeleteGameObject(CGameObject* pGameObj)
{
	UnlinkLinkedList(&m_pGameObjLinkHead, &m_pGameObjLinkTail, &pGameObj->inGameLinkNode);
	m_dwGameObjNum--;
	delete pGameObj;
}

void CGame::DeleteAllGameObjects()
{
	while (m_pGameObjLinkHead)
	{
		CGameObject* pGameObj = (CGameObject*)m_pGameObjLinkHead->pData;
		DeleteGameObject(pGameObj);
	}
}
