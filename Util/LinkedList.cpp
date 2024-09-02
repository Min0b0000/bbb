#include "LinkedList.h"

void LinkToLinkedList(LINK_NODE** ppHead, LINK_NODE** ppTail, LINK_NODE* pNew)
{
	if (!(*ppHead))
	{
		(*ppHead) = (*ppTail) = pNew;
		pNew->pPrev = nullptr;
		pNew->pNext = nullptr;
	} else
	{
		pNew->pPrev = (*ppTail);
		(*ppTail)->pNext = pNew;
		(*ppTail) = pNew;
		pNew->pNext = nullptr;
	}
}

void LinkToLinkedListFIFO(LINK_NODE** ppHead, LINK_NODE** ppTail, LINK_NODE* pNew)
{
	if (!(*ppHead))
	{
		(*ppHead) = (*ppTail) = pNew;
		pNew->pPrev = nullptr;
		pNew->pNext = nullptr;
	} else
	{
		pNew->pNext = (*ppHead);
		(*ppHead)->pPrev = pNew;
		(*ppHead) = pNew;
		pNew->pPrev = nullptr;
	}
}

void UnlinkLinkedList(LINK_NODE** ppHead, LINK_NODE** ppTail, LINK_NODE* pDel)
{
	LINK_NODE* pPrev = pDel->pPrev;
	LINK_NODE* pNext = pDel->pNext;

	if (pNext)
	{
		pNext->pPrev = pPrev;
	} else
	{
		(*ppTail) = pPrev;
	}

	if (pPrev)
	{
		pPrev->pNext = pNext;
	} else
	{
		(*ppHead) = pNext;
	}

	pDel->pNext = nullptr;
	pDel->pPrev = nullptr;
}
