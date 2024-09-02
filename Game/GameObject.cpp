#include "GameObject.h"
#include "Game.h"
#include "../Renderer/DDrawRenderer.h"



CGameObject::CGameObject()
{
}

CGameObject::~CGameObject()
{
    Cleanup();
}

BOOL CGameObject::Initialize(CGame* pGame)
{
    m_pGame             = pGame;
    m_pDDrawRenderer    = m_pGame->INL_GetDDrawRenderer();
    inGameLinkNode.pData = this;
    onRenderLinkNode.pData = this;
    return TRUE;
}

BOOL CGameObject::InsertActor(void(*pfnUpdate)(CGameObject* pThis), void* pRenderObject, GAMEOBJECT_RENDER_TYPE GameObjRenderType)
{
    m_pRenderObject     = pRenderObject;
    m_GameObjRenderType = GameObjRenderType;
    m_pfnUpdate         = pfnUpdate;
    return TRUE;
}

void CGameObject::Run()
{
    if (m_pfnUpdate)
    {
        m_pfnUpdate(this);
    }
}

void CGameObject::Render()
{

}

void CGameObject::SetWorldPosition(float fX, float fY, float fZ)
{
    m_f3Position = { fX, fY, fZ };
    /* if (GAMEOBJ_RENDER_SOFTWAREMESHOBJ == m_GameObjRenderType)
    {
        m_pDDrawRenderer->UpdateMeshTransform(m_pRenderObject, &m_f3Position, NULL, NULL);
    } */
}

float CGameObject::GetDepth() const
{
    float fDepth = 0.0f;
    switch (m_GameObjRenderType)
    {
    case GAMEOBJ_RENDER_UNKNOWN:
        break;
    case GAMEOBJ_RENDER_SPRITE:
    case GAMEOBJ_RENDER_TILEMAP:
        fDepth = m_f3Position.z;
        break;
    default:
        break;
    } return fDepth;
}

void* CGameObject::INL_GetRenderObject(GAMEOBJECT_RENDER_TYPE* pGameObjRenderType)
{
    if (pGameObjRenderType)
    {
        (*pGameObjRenderType) = m_GameObjRenderType;
    } return m_pRenderObject;
}

void CGameObject::Cleanup()
{
    if (m_pRenderObject)
    {
        DeleteRenderObject();
    }

    m_pfnUpdate = nullptr;
    m_f3Position = {};
    m_pDDrawRenderer = nullptr;
    m_pGame = nullptr;
}

void CGameObject::DeleteRenderObject()
{
    switch (m_GameObjRenderType)
    {
    case GAMEOBJ_RENDER_UNKNOWN:
        __debugbreak();
        break;
    case GAMEOBJ_RENDER_SPRITE:
    case GAMEOBJ_RENDER_TILEMAP:
        m_pDDrawRenderer->DeleteSpriteObject(m_pRenderObject);
        break;
    default:
        __debugbreak();
        break;
    };
    m_pRenderObject = nullptr;
    m_GameObjRenderType = GAMEOBJ_RENDER_UNKNOWN;
}
