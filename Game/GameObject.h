#pragma once

#include "GameTypedef.h"



class CGame;
class CDDrawRenderer;

class CGameObject
{
public:
	CGameObject();
	~CGameObject();

	BOOL Initialize(CGame* pGame);
	BOOL InsertActor(void(*pfnUpdate)(CGameObject* pThis), void* pRenderObject, GAMEOBJECT_RENDER_TYPE GameObjRenderType);

	void Run();
	void Render();

	const float3* GetWorldPosition() const { return &m_f3Position; };
	void SetWorldPosition(float fX, float fY, float fZ);

	float GetDepth() const;

	CGame* INL_GetGame() { return m_pGame; };
	void* INL_GetRenderObject(GAMEOBJECT_RENDER_TYPE* pGameObjRenderType);

	void Cleanup();

public:
	LINK_NODE inGameLinkNode;
	LINK_NODE onRenderLinkNode;
	
private:
	void DeleteRenderObject();

private:
	CGame*			m_pGame = nullptr;
	CDDrawRenderer* m_pDDrawRenderer = nullptr;

	float3 m_f3Position = {};
	void(*m_pfnUpdate)(CGameObject*) = nullptr;

	void* m_pRenderObject = nullptr;
	GAMEOBJECT_RENDER_TYPE m_GameObjRenderType = GAMEOBJ_RENDER_UNKNOWN;
};