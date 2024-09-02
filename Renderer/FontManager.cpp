#include "FontManager.h"



CFontManager::CFontManager()
{
}

CFontManager::~CFontManager()
{
	Cleanup();
}

BOOL CFontManager::Initialize(CDDrawRenderer* pDDrawRenderer)
{
	m_pDDrawRenderer = pDDrawRenderer;
	return TRUE;
}

void CFontManager::Cleanup()
{

}
