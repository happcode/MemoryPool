#include "StdAfx.h"
#include "MemoryPoolManag.h"
#include "MemoryChunk.h"


CMemoryPoolManag::CMemoryPoolManag( const size_t& nMemoryPoolSize, const size_t& nOrignBlockSize )
	: m_nMemoryPoolSize(0)
	, m_nMemoryPoolBlockSize(0)
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
	// �����ڴ棨�ڴ��͹����ࣩ������ԭʼ�ڴ���䵽��������
	MallocMemory(nMemoryPoolSize);
}

void CMemoryPoolManag::Release()
{
	// 1.�ͷ�ԭʼ�ڴ����Դ
	MemoryChunk* pChunk = m_pFirstChunk;
	while (pChunk != NULL)
	{
		if (pChunk->m_bIsBlockHeader)
		{
			free(pChunk->m_pData);
		}
		pChunk = pChunk->m_pNext;
	}

	// 2.�ͷŹ����ڴ���(Ҳ����һ��һ��������ڴ����ɵ�)��Դ
	MemoryChunk* pNextChunks = NULL;
	pChunk = m_pFirstChunk;
	while (pChunk != NULL)
	{
		// Ѱ����һ��chunk��
		pNextChunks = pChunk->m_pNext;
		while (pNextChunks != NULL && !pNextChunks->m_bIsBlockHeader)
		{
			pNextChunks = pNextChunks->m_pNext;
		}

		// �ͷ���һ��chunk��
		free(pChunk);
		pChunk = NULL;
		pChunk = pNextChunks;
	}
}

void* CMemoryPoolManag::MallocMemory( const size_t& nMallocMemorySize )
{
	// 1.�����������Ҫ��Ϊ���ٿ�
	size_t nBlockCount = GetBlockCount(nMallocMemorySize);
	// 2.����ʵ����Ҫ�����ڴ��С
	size_t nTotalMallocMemorySize = nBlockCount*m_nOrignBlockSize;

	// 3.����ԭʼ�����ڴ��
	BYTE*  pOrignMemory = (BYTE*)malloc(nTotalMallocMemorySize);
	assert(pOrignMemory !=NULL );

	// 4.���������ڴ������
	m_nMemoryPoolSize += nTotalMallocMemorySize;
	m_nMemoryPoolBlockSize += nBlockCount;
	std::memset(pOrignMemory, 0, nTotalMallocMemorySize);

	// 5.��ԭʼ�ڴ���䵽�ڴ�������
	DistributeMemoryToChunk(pOrignMemory, nBlockCount);

	return pOrignMemory;
}

void CMemoryPoolManag::DistributeMemoryToChunk( BYTE* pOrignMemory, const size_t& nBlockCount )
{
	// 1.����������
	MemoryChunk* pChunks = (MemoryChunk*)malloc(nBlockCount*sizeof(MemoryChunk));
	assert(pChunks != NULL);

	// 2.���ڴ����ݷ���������У�����ʼ�����ݿ��С�͵�ַ,ʹ��������������
	MemoryChunk* pChunk = NULL;
	for (size_t i = 0; i < nBlockCount; ++i)
	{
		pChunk = &pChunks[i];
		assert(pChunk != NULL);
		pChunk->Initial();

		if (0 == i)
		{
			// ��ΪpOrignMemory�������ڴ�飬���Լ�¼ͷָ�룬���ͷ���Դ��ʱ��free(pChunk->data)����
			pChunk->m_bIsBlockHeader = true;
			pChunk->m_pData = pOrignMemory;
		}

		if (NULL == m_pFirstChunk)
		{			 
			// �ڴ�س�ʼ��ʱ���룬�����ڴ������ʱ�������
			m_pFirstChunk = pChunk;
			//m_pCurrentChunk = pChunk;
		}
		else
		{
			m_pLastChunk->m_pNext = pChunk;			
		}

		m_pLastChunk = pChunk;
	}

	// 3.���¼��������ڴ�������еĹ�����ڴ���С
	ReCalcAllBlockSize();
}

void* CMemoryPoolManag::GetMemory( const size_t& nNeedMemorySize )
{
	// ���������ڴ����
	size_t nNeedBlockCount = GetBlockCount(nNeedMemorySize);

	MemoryChunk* pChunk = m_pFirstChunk;
	// �жϵ�ǰ������е�ʣ���ڴ滹����������ģ���������������
	while (pChunk != NULL)
	{		
		if (0 == pChunk->m_nUsed && pChunk->m_nSize > nNeedMemorySize)
		{
			// 1.���Ȳ鿴���⼸���ڴ����Ƿ���ڱ�������ڴ�飬��������
			MemoryChunk* pAllocated = IsAllocated(pChunk, nNeedBlockCount);
			if (pAllocated != NULL)
			{
				pChunk = SkipChunks(pAllocated, nNeedBlockCount);
				continue;
			}

			// �Ѿ�������nNeedBlockCount����ڴ汻����
			ChangeChunkStat(pChunk, nNeedBlockCount, true);
			pChunk->m_nUsed = nNeedBlockCount*m_nOrignBlockSize;
			// �����ڴ��ָ��
			return pChunk->m_pData;
		}
		else
		{
			// �ڴ治������ת����һ�����õ��ڴ�������(�����Ѿ����˶����ڴ�)
			size_t nSkipChunkCount = GetBlockCount(pChunk->m_nUsed);			
			SkipChunks(pChunk, nSkipChunkCount);
		}
	}

	// ���·����ڴ棨����ʽ��
	return MallocMemory(nNeedMemorySize);	
}

void CMemoryPoolManag::FreeMemory( MemoryChunk* pMemory )
{
	// ����Ҫ�ͷŵ��ڴ����
	size_t nFreeBlockCount = GetBlockCount(pMemory->m_nUsed);
	// ->�ı�ռ�ñ�־
	ChangeChunkStat(pMemory, nFreeBlockCount, false);
	// ->����������
	memset(pMemory, 0, pMemory->m_nUsed);
}

void CMemoryPoolManag::ChangeChunkStat(MemoryChunk* pChunk, int nBlockCount, bool bAllocated)
{
	MemoryChunk* pTemp = NULL;
	pTemp = pChunk;
	for (size_t i = 0; i < nBlockCount; ++i)
	{
		assert(pTemp != NULL);
		pTemp->m_bIsAllocate = bAllocated;
		pTemp = pTemp->m_pNext;
	}
}

void CMemoryPoolManag::ReCalcAllBlockSize()
{
	MemoryChunk* pTemp = m_pFirstChunk;
	assert(pTemp != NULL);
	for (size_t i = 0; i < m_nMemoryPoolBlockSize; ++i)
	{
		// ���¼���ÿ���ڴ��С
		size_t nOffset = i*m_nOrignBlockSize;
		pTemp->m_nSize = m_nMemoryPoolSize - nOffset;	
		pTemp = pTemp->m_pNext;
	}
}

MemoryChunk* CMemoryPoolManag::SkipChunks(MemoryChunk* pChunk, int nSkipChunkCount)
{
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
		if (pTemp->m_bIsAllocate)
		{
			return pTemp;
		}
		pTemp = pTemp->m_pNext;
	}

	return NULL;
}
