#include "HashTable.h"



CHashTable::CHashTable(void)
{
}

CHashTable::~CHashTable(void)
{
	Cleanup();
}

BOOL CHashTable::Initialize(DWORD dwMaxBucketNum, DWORD dwMaxKeySize)
{
	m_dwMaxKeyDataSize = dwMaxKeySize;
	m_dwMaxBucketNum = dwMaxBucketNum;
	m_pBucketDescTable = (HASH_BUCKET_DESC*)malloc(sizeof(HASH_BUCKET_DESC) * dwMaxBucketNum);
	memset(m_pBucketDescTable, 0, sizeof(HASH_BUCKET_DESC) * dwMaxBucketNum);
	return TRUE;
}

DWORD CHashTable::Select(void** ppOutItemList, DWORD dwMaxItemNum, const void* pKeyData, DWORD dwSize)
{
	DWORD		dwIndex = CreateKey(pKeyData, dwSize, m_dwMaxBucketNum);
	HASH_BUCKET_DESC* pBucketDesc = m_pBucketDescTable + dwIndex;

	LINK_NODE* pCur = pBucketDesc->pBucketLinkHead;
	HASH_BUCKET* pBucket;

	DWORD dwSelectedItemNum = 0;

	while (pCur)
	{
		if (!dwMaxItemNum)
		{
			goto lb_return;
		}

		pBucket = (HASH_BUCKET*)pCur->pData;

		if (pBucket->dwSize != dwSize)
		{
			goto lb_next;
		}

		if (memcmp(pBucket->pKeyData, pKeyData, dwSize))
		{
			goto lb_next;
		}

		ppOutItemList[dwSelectedItemNum] = (void*)pBucket->pItem;
		dwSelectedItemNum++;
		dwMaxItemNum--;

	lb_next:
		pCur = pCur->pNext;
	}

lb_return:
	return dwSelectedItemNum;
}

void* CHashTable::Insert(const void* pItem, const void* pKeyData, DWORD dwSize)
{
	void* pSearchHandle = nullptr;
	HASH_BUCKET_DESC* pBucketDesc;
	HASH_BUCKET* pBucket;
	DWORD dwBucketMemSize;
	DWORD dwIndex;

	if (dwSize > m_dwMaxKeyDataSize)
	{
		__debugbreak();
		goto lb_return;
	}

	dwBucketMemSize = sizeof(HASH_BUCKET) + m_dwMaxKeyDataSize;
	pBucket = (HASH_BUCKET*)malloc(dwBucketMemSize);
	dwIndex = CreateKey(pKeyData, dwSize, m_dwMaxBucketNum);
	pBucketDesc = m_pBucketDescTable + dwIndex;

	pBucket->pItem = pItem;
	pBucket->dwSize = dwSize;
	pBucket->pBucketDesc = pBucketDesc;
	pBucket->linkNode.pPrev = NULL;
	pBucket->linkNode.pNext = NULL;
	pBucket->linkNode.pData = pBucket;
	memcpy(pBucket->pKeyData, pKeyData, dwSize);

	LinkToLinkedListFIFO(&pBucketDesc->pBucketLinkHead, &pBucketDesc->pBucketLinkTail, &pBucket->linkNode);
	pBucketDesc->dwLinkNum++;
	m_dwItemNum++;

	pSearchHandle = pBucket;

lb_return:
	return pSearchHandle;
}

void CHashTable::Delete(const void* pSearchHandle)
{
	HASH_BUCKET* pBucket = (HASH_BUCKET*)pSearchHandle;
	HASH_BUCKET_DESC* pBucketDesc = pBucket->pBucketDesc;

	UnlinkLinkedList(&pBucketDesc->pBucketLinkHead, &pBucketDesc->pBucketLinkTail, &pBucket->linkNode);
	pBucketDesc->dwLinkNum--;
	m_dwItemNum--;

	free(pBucket);
}

DWORD CHashTable::GetMaxBucketNum(void) const
{
	return m_dwMaxBucketNum;
}

void CHashTable::DeleteAll(void)
{
	HASH_BUCKET* pBucket;

	for (DWORD i = 0; m_dwMaxBucketNum > i; ++i)
	{
		while (m_pBucketDescTable[i].pBucketLinkHead)
		{
			pBucket = (HASH_BUCKET*)m_pBucketDescTable[i].pBucketLinkHead->pData;
			Delete(pBucket);
		}
	}
}

DWORD CHashTable::GetAllItems(void** ppOutItemList, DWORD dwMaxItemNum, BOOL* pbOutInsufficient) const
{
	HASH_BUCKET* pBucket;
	DWORD	dwItemNum = 0;
	LINK_NODE* pCur;

	(*pbOutInsufficient) = FALSE;

	for (DWORD i = 0; m_dwMaxBucketNum > i; ++i)
	{
		pCur = m_pBucketDescTable[i].pBucketLinkHead;
		while (pCur)
		{
			pBucket = (HASH_BUCKET*)pCur->pData;

			if (dwItemNum >= dwMaxItemNum)
			{
				(*pbOutInsufficient) = TRUE;
				goto lb_return;
			}

			ppOutItemList[dwItemNum] = (void*)pBucket->pItem;
			dwItemNum++;

			pCur = pCur->pNext;
		}
	}

lb_return:
	return dwItemNum;
}

DWORD CHashTable::GetKeyPtrAndSize(void** ppOutKeyPtr, const void* pSearchHandle) const
{
	HASH_BUCKET* pBucket = (HASH_BUCKET*)pSearchHandle;
	DWORD	dwSize;

	(*ppOutKeyPtr) = pBucket->pKeyData;
	dwSize = pBucket->dwSize;

	return dwSize;
}

DWORD CHashTable::GetItemNum(void) const
{
	return m_dwItemNum;
}

void CHashTable::Cleanup(void)
{
	if (m_dwItemNum)
	{
		__debugbreak();
	}

	DeleteAll();

	if (m_pBucketDescTable)
	{
		free(m_pBucketDescTable);
		m_pBucketDescTable = nullptr;
	}
}

DWORD CHashTable::CreateKey(const void* pData, DWORD dwSize, DWORD dwBucketNum)
{
	DWORD dwKeyData = 0;
	const BYTE* pEntry = (BYTE*)pData;

	if (!dwSize)
	{
		__debugbreak();
	}

	if (dwSize & 0x0001)
	{
		dwKeyData += (DWORD)(*(BYTE*)pEntry);
		pEntry++;
		dwSize--;
	}

	if (!dwSize)
	{
		goto lb_exit;
	}

	if (dwSize & 0x0002)
	{
		dwKeyData += (DWORD)(*(WORD*)pEntry);
		pEntry += 2;
		dwSize -= 2;
	}

	if (!dwSize)
	{
		goto lb_exit;
	}

	dwSize = (dwSize >> 2); // div 4

	for (DWORD i = 0; dwSize > i; ++i)
	{
		dwKeyData += *(DWORD*)pEntry;
		pEntry += 4;
	}

lb_exit:
	DWORD dwIndex = dwKeyData % dwBucketNum;
	return dwIndex;
}
