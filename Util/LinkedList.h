#pragma once



struct LINK_NODE
{
	void* pData;
	LINK_NODE* pPrev;
	LINK_NODE* pNext;
};

void LinkToLinkedList(LINK_NODE** ppHead, LINK_NODE** ppTail, LINK_NODE* pNew);
void LinkToLinkedListFIFO(LINK_NODE** ppHead, LINK_NODE** ppTail, LINK_NODE* pNew);
void UnlinkLinkedList(LINK_NODE** ppHead, LINK_NODE** ppTail, LINK_NODE* pDel);
