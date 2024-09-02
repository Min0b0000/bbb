#include "FnGameObjectUpdate.h"
#include "Game.h"
#include "GameObject.h"
#include "../Renderer/DDrawRenderer.h"



void FnPlayerUpdate(CGameObject* pThis)
{
	CGame* pGame = pThis->INL_GetGame();
	CDDrawRenderer* pRnd = pThis->INL_GetGame()->INL_GetDDrawRenderer();
	pRnd->PlaySpriteAnimation(pThis->INL_GetRenderObject(NULL), pGame->GetCurTick());
}
