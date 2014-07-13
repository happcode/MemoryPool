#include "StdAfx.h"
#include "MemoryPoolManag.h"


CMemoryPoolManag::CMemoryPoolManag( const size_t& nMemoryPoolSize, const size_t& nOrignBlockSize )
	: m_nMemoryPoolSize(0)
	, m_nMemoryPoolBlockSize(0)
	, m_nOrignBlockSize(nOrignBlockSize)
	, m_pFirstChunk(NULL)
	, m_pLastChunk(NULL)
{
	Initial(nMemoryPoolSize);
}

CMemoryPoolManag::~CMemoryPoolManag(void)
{
	Release();
}

void CMemoryPoolManag::Initial(const size_t& nMemoryPoolSize)
{
	// 分配内存（内存块和管理类），并将原始内存分配到管理类中
	MallocMemory(nMemoryPoolSize);
}

void CMemoryPoolManag::Release()
{
	// 1.释放原始内存块资源
	MemoryChunk* pChunk = m_pFirstChunk;
	while (pChunk != NULL)
	{
		if (pChunk->m_bIsBlockHeader)
		{
			free(pChunk->m_pData);
		}
		pChunk = pChunk->m_pNext;
	}

	// 2.释放管理内存类(也是由一块一块的连续内存块组成的)资源
	MemoryChunk* pNextChunks = NULL;
	pChunk = m_pFirstChunk;
	while (pChunk != NULL)
	{
		// 寻找下一个chunk块
		pNextChunks = pChunk->m_pNext;
		while (pNextChunks != NULL && !pNextChunks->m_bIsBlockHeader)
		{
			pNextChunks = pNextChunks->m_pNext;
		}

		// 释放上一个chunk块
		free(pChunk);
		pChunk = NULL;
		pChunk = pNextChunks;
	}
}

void* CMemoryPoolManag::MallocMemory( const size_t& nMallocMemorySize )
{
	// 1.先算出至少需要分为多少块
	size_t nBlockCount = GetBlockCount(nMallocMemorySize);
	// 2.再算实际需要分配内存大小
	size_t nTotalMallocMemorySize = nBlockCount*m_nOrignBlockSize;

	// 3.申请原始连续内存块
	BYTE*  pOrignMemory = (BYTE*)malloc(nTotalMallocMemorySize);
	assert(pOrignMemory !=NULL );

	// 4.保存申请内存的增量
	m_nMemoryPoolSize += nTotalMallocMemorySize;
	m_nMemoryPoolBlockSize += nBlockCount;
	std::memset(pOrignMemory, 0, nTotalMallocMemorySize);

	// 5.将原始内存分配到内存管理块中(返回第一块管理类）
	MemoryChunk* pFirst = DistributeMemoryToChunk(pOrignMemory, nBlockCount, nTotalMallocMemorySize);
	if (pFirst != m_pFirstChunk)
	{
		pFirst->m_nUsed = nTotalMallocMemorySize;
		return pFirst->m_pData;
	}

	return m_pFirstChunk->m_pData;
}

MemoryChunk* CMemoryPoolManag::DistributeMemoryToChunk( BYTE* pOrignMemory, const size_t& nBlockCount, const size_t& nTotalMallocMemorySize )
{
	// 1.分配管理块类
	MemoryChunk* pChunks = (MemoryChunk*)malloc(nBlockCount*sizeof(MemoryChunk));
	assert(pChunks != NULL);

	// 2.将内存数据放入管理类中，并初始化数据块大小和地址,使用链表连接起来
	MemoryChunk* pChunk = NULL;
	for (size_t i = 0; i < nBlockCount; ++i)
	{
		// 每块内存管理的内存起始地址(即偏移量)		
		pChunk = &pChunks[i];
		assert(pChunk != NULL);
		size_t nOffset = i*m_nOrignBlockSize;
		pChunk->Initial();
		pChunk->m_pData = &pOrignMemory[nOffset];
		pChunk->m_nSize = nTotalMallocMemorySize - nOffset;

		if (0 == i)
		{
			// 因为pOrignMemory是连续内存块，所以记录头指针，在释放资源的时候，free(pChunk->data)即可
			pChunk->m_bIsBlockHeader = true;			
		}

		if (NULL == m_pFirstChunk)
		{			 
			// 内存池初始化时进入，增加内存块申请时不会进入
			m_pFirstChunk = pChunk;			
		}
		else
		{
			m_pLastChunk->m_pNext = pChunk;			
		}

		m_pLastChunk = pChunk;
	}

	// 返回分配内存后第一块内存管理类
	return &pChunks[0];
	// 3.重新计算所有内存管理类中的管理的内存块大小
	//ReCalcAllBlockSize();要保证返回给用户连续的内存块
}

void* CMemoryPoolManag::GetMemory( const size_t& nNeedMemorySize )
{
	// 计算所需内存块数
	size_t nNeedBlockCount = GetBlockCount(nNeedMemorySize);

	MemoryChunk* pChunk = m_pFirstChunk;
	// 判断当前管理快中的剩余内存还够不够分配的，不够则重新申请
	while (pChunk != NULL)
	{		
		if (0 == pChunk->m_nUsed && pChunk->m_nSize > nNeedMemorySize)
		{
			// 1.首先查看在这几块内存中是否存在被分配的内存块，有则跳过
			MemoryChunk* pAllocated = IsAllocated(pChunk, nNeedBlockCount);
			if (pAllocated != NULL)
			{
				pChunk = SkipChunks(pAllocated, nNeedBlockCount);
				continue;
			}

			// 找到可用的连续内存空间
			pChunk->m_nUsed = nNeedBlockCount*m_nOrignBlockSize;			
			return pChunk->m_pData;
		}
		else
		{
			// 内存不够，跳转到下一个可用的内存管理块中(计算已经用了多少内存)
			size_t nSkipChunkCount = GetBlockCount(pChunk->m_nUsed);			
			pChunk = SkipChunks(pChunk, nSkipChunkCount);
		}
	}

	// 重新分配内存（增量式）
	return MallocMemory(nNeedMemorySize);
}

void CMemoryPoolManag::FreeMemory( void* pMemory )
{
	// 找到管理内存的类
	MemoryChunk* pTemp = FindMemoryBlockByData(pMemory);
	assert(pTemp != NULL);
	
	// ->将数据置零
	memset(pMemory, 0, pTemp->m_nUsed);
	pTemp->m_nUsed = 0;
}

void CMemoryPoolManag::ReCalcAllBlockSize()
{
	MemoryChunk* pTemp = m_pFirstChunk;
	assert(pTemp != NULL);
	for (size_t i = 0; i < m_nMemoryPoolBlockSize; ++i)
	{
		// 重新计算每块内存大小
		size_t nOffset = i*m_nOrignBlockSize;
		pTemp->m_nSize = m_nMemoryPoolSize - nOffset;	
		pTemp = pTemp->m_pNext;
	}
}

MemoryChunk* CMemoryPoolManag::SkipChunks(MemoryChunk* pChunk, int nSkipChunkCount)
{
	if (0 == nSkipChunkCount)
	{
		return pChunk->m_pNext;
	}
	for (size_t i = 0; i < nSkipChunkCount; ++i)
	{
		assert(pChunk != NULL);
		pChunk = pChunk->m_pNext;
	}
	return pChunk;
}

MemoryChunk* CMemoryPoolManag::IsAllocated( MemoryChunk* pChunk, size_t nNeedBlockCount )
{
	MemoryChunk* pTemp = pChunk;
	for (size_t i = 0; i < nNeedBlockCount; ++i)
	{
		if (pTemp->m_nUsed != 0)
		{
			return pTemp;
		}
		pTemp = pTemp->m_pNext;
	}

	return NULL;
}

MemoryChunk* CMemoryPoolManag::FindMemoryBlockByData( void* pMemory )
{
	MemoryChunk* pTemp = m_pFirstChunk;
	while (pTemp != NULL)
	{
		if (pTemp->m_pData == pMemory)
		{
			return pTemp;
		}
		pTemp = pTemp->m_pNext;
	}

	return NULL;
}
