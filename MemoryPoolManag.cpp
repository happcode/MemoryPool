#include "StdAfx.h"
#include "MemoryPoolManag.h"
#include "MemoryChunk.h"


CMemoryPoolManag::CMemoryPoolManag( const size_t& nMemoryPoolSize, const size_t& nOrignBlockSize )
	: m_nMemoryPoolSize(0)
	, m_nOrignBlockSize(nOrignBlockSize)
	, m_pFirstChunk(NULL)
	, m_pCurrentChunk(NULL)
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
		if (pChunk->m_bIsAllocate)
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
		while (pNextChunks != NULL && !pNextChunks->m_bIsAllocate)
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
	std::memset(pOrignMemory, 0, nTotalMallocMemorySize);

	// 5.将原始内存分配到内存管理块中
	DistributeMemoryToChunk(pOrignMemory, nBlockCount, nTotalMallocMemorySize);

	return pOrignMemory;
}

void CMemoryPoolManag::DistributeMemoryToChunk( BYTE* pOrignMemory, const size_t& nBlockCount, const size_t& nTotalMallocMemorySize )
{
	// 1.分配管理块类
	MemoryChunk* pChunks = (MemoryChunk*)malloc(nBlockCount*sizeof(MemoryChunk));
	assert(pChunks != NULL);

	// 2.将内存数据放入管理类中，并初始化数据块大小和地址
	MemoryChunk* pChunk = NULL;
	for (size_t i = 0; i < nBlockCount; ++i)
	{
		pChunk = &pChunks[i];
		assert(pChunk != NULL);
		pChunk->Initial();

		if (0 == i)
		{
			// 因为pOrignMemory是连续内存块，所以记录头指针，在释放资源的时候，free(pChunk->data)即可
			pChunk->m_bIsAllocate = true;
		}

		if (NULL == m_pFirstChunk)
		{			 
			// 内存池初始化时进入，增加内存块申请时不会进入
			m_pFirstChunk = pChunk;
			m_pCurrentChunk = pChunk;
		}
		else
		{
			m_pLastChunk->m_pNext = pChunk;			
		}

		m_pLastChunk = pChunk;
		
		// 分配数据
		size_t nOffset = i*m_nOrignBlockSize;
		pChunk->m_nSize = nTotalMallocMemorySize - nOffset;
		pChunk->m_pData = &pOrignMemory[nOffset];
	}
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
			// 返回内存块指针
			pChunk->m_nUsed = nNeedBlockCount*m_nOrignBlockSize;
			return pChunk->m_pData;
		}
		else
		{
			// 内存不够，跳转到下一个可用的内存管理块中(计算已经用了多少内存)
			size_t nSkipChunkCount = GetBlockCount(pChunk->m_nUsed);			
			for (size_t i = 0; i < nSkipChunkCount; ++i)
			{
				pChunk = pChunk->m_pNext;
			}
		}
	}

	// 重新分配内存（增量式）
	return MallocMemory(nNeedMemorySize);	
}

void CMemoryPoolManag::FreeMemory( void* pMemory )
{
	throw std::exception("The method or operation is not implemented.");
}
