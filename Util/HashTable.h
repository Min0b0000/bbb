#pragma once

#include "../framework.h"



struct LINK_NODE;

struct HASH_BUCKET_DESC
{
	LINK_NODE*	pBucketLinkHead;
	LINK_NODE*	pBucketLinkTail;
	DWORD		dwLinkNum;
};

struct HASH_BUCKET
{
	const void*		pItem;
	HASH_BUCKET_DESC* pBucketDesc;
	LINK_NODE		linkNode;
	DWORD			dwSize;
	char			pKeyData[0];
};

class CHashTable
{
public:
	CHashTable(void);
	~CHashTable(void);

	BOOL Initialize(DWORD dwMaxBucketNum, DWORD dwMaxKeySize);
	DWORD Select(void** ppOutItemList, DWORD dwMaxItemNum, const void* pKeyData, DWORD dwSize);
	void* Insert(const void* pItem, const void* pKeyData, DWORD dwSize);
	void Delete(const void* pSearchHandle);
	DWORD GetMaxBucketNum(void) const;
	void DeleteAll(void);
	DWORD GetAllItems(void** ppOutItemList, DWORD dwMaxItemNum, BOOL* pbOutInsufficient) const;
	DWORD GetKeyPtrAndSize(void** ppOutKeyPtr, const void* pSearchHandle) const;
	DWORD GetItemNum(void) const;
	void Cleanup(void);

private:
	DWORD CreateKey(const void* pData, DWORD dwSize, DWORD dwBucketNum);

private:
	HASH_BUCKET_DESC* m_pBucketDescTable = nullptr;
	DWORD m_dwMaxBucketNum = 0;
	DWORD m_dwMaxKeyDataSize = 0;
	DWORD m_dwItemNum = 0;
};