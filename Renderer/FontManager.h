#pragma once

#include "../framework.h"
#include "../Typedef.h"



class CDDrawRenderer;

class CFontManager
{
public:
	CFontManager();
	~CFontManager();

	BOOL Initialize(CDDrawRenderer* pDDrawRenderer);

	FONT_HANDLE* CreateFontHandle(const WCHAR* wcsFileName);
	void DeleteFontHandle(FONT_HANDLE* pFont);



private:
	void Cleanup();

private:
	CDDrawRenderer* m_pDDrawRenderer;

	LINK_NODE* m_pFontLinkHead;
	LINK_NODE* m_pFontLinkTail;
	


};