#pragma once

#include "GameTypedef.h"



class CGameObject;
class CDDrawRenderer;

class CGame
{
public:
	CGame(void);
	~CGame(void);

	BOOL Initialize(HWND hWnd);

	void Run(void);

	void CreateVirtualResolution(WORD width, WORD height);
	void DeleteVirtualResolution(void);

	void UpdateWindowPos(void);
	void UpdateWindowSize(void);

	const float3* GetCamPosition() const { return &m_f3CamPosition; };
	void SetCamPosition(float fX, float fY, float fZ);

	void InsertRenderList(CGameObject* pGameObj, float fDepth);

	void CreatePlayer(float x, float y);

	ULONGLONG GetCurTick() const { return m_CurTick; };

	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseWheel(UINT message, WPARAM wParam, LPARAM lParam);
	void OnMouseButtonDown(UINT message, WPARAM wParam, LPARAM lParam);
	void OnMouseButtonUp(UINT message, WPARAM wParam, LPARAM lParam);
	void OnKeyDown(UINT message, WPARAM wParam, LPARAM lParam);
	void OnKeyUp(UINT message, WPARAM wParam, LPARAM lParam);

	CDDrawRenderer* INL_GetDDrawRenderer() { return m_pDDrawRenderer; };

private:
	void Cleanup();

	BOOL Update(ULONGLONG CurTick);
	void Render();

	CGameObject* CreateGameObject();
	void DeleteGameObject(CGameObject* pGameObj);
	void DeleteAllGameObjects();

private:
	HWND m_hWnd = NULL;
	CDDrawRenderer* m_pDDrawRenderer = nullptr;

	float3 m_f3CamPosition = {};

	LINK_NODE* m_pGameObjLinkHead = nullptr;
	LINK_NODE* m_pGameObjLinkTail = nullptr;
	DWORD m_dwGameObjNum = 0;

	LINK_NODE* m_pRenderSortLinkHead = nullptr;
	LINK_NODE* m_pRenderSortLinkTail = nullptr;
	DWORD m_dwRndObjNum = 0;

	ULONGLONG m_CurTick = 0;
	ULONGLONG m_PrvFrameCheckTick = 0;
	ULONGLONG m_PrvUpdateTick = 0;
	DWORD m_dwFrameCount = 0;
	DWORD m_dwFPS = 0;
};