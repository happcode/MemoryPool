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
	// �����ڴ棨�ڴ��͹����ࣩ������ԭʼ�ڴ���䵽��������
	MallocMemory(nMemoryPoolSize);
}

void CMemoryPoolManag::Release()
{
	// 1.�ͷ�ԭʼ�ڴ����Դ
	MemoryChunk* pChunk = m_pFirstChunk;
	while (pChunk != NULL)
	{
		if (pChunk->m_bIsAllocate)
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
		while (pNextChunks != NULL && !pNextChunks->m_bIsAllocate)
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
	std::memset(pOrignMemory, 0, nTotalMallocMemorySize);

	// 5.��ԭʼ�ڴ���䵽�ڴ�������
	DistributeMemoryToChunk(pOrignMemory, nBlockCount, nTotalMallocMemorySize);

	return pOrignMemory;
}

void CMemoryPoolManag::DistributeMemoryToChunk( BYTE* pOrignMemory, const size_t& nBlockCount, const size_t& nTotalMallocMemorySize )
{
	// 1.����������
	MemoryChunk* pChunks = (MemoryChunk*)malloc(nBlockCount*sizeof(MemoryChunk));
	assert(pChunks != NULL);

	// 2.���ڴ����ݷ���������У�����ʼ�����ݿ��С�͵�ַ
	MemoryChunk* pChunk = NULL;
	for (size_t i = 0; i < nBlockCount; ++i)
	{
		pChunk = &pChunks[i];
		assert(pChunk != NULL);
		pChunk->Initial();

		if (0 == i)
		{
			// ��ΪpOrignMemory�������ڴ�飬���Լ�¼ͷָ�룬���ͷ���Դ��ʱ��free(pChunk->data)����
			pChunk->m_bIsAllocate = true;
		}

		if (NULL == m_pFirstChunk)
		{			 
			// �ڴ�س�ʼ��ʱ���룬�����ڴ������ʱ�������
			m_pFirstChunk = pChunk;
			m_pCurrentChunk = pChunk;
		}
		else
		{
			m_pLastChunk->m_pNext = pChunk;			
		}

		m_pLastChunk = pChunk;
		
		// ��������
		size_t nOffset = i*m_nOrignBlockSize;
		pChunk->m_nSize = nTotalMallocMemorySize - nOffset;
		pChunk->m_pData = &pOrignMemory[nOffset];
	}
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
			// �����ڴ��ָ��
			pChunk->m_nUsed = nNeedBlockCount*m_nOrignBlockSize;
			return pChunk->m_pData;
		}
		else
		{
			// �ڴ治������ת����һ�����õ��ڴ�������(�����Ѿ����˶����ڴ�)
			size_t nSkipChunkCount = GetBlockCount(pChunk->m_nUsed);			
			for (size_t i = 0; i < nSkipChunkCount; ++i)
			{
				pChunk = pChunk->m_pNext;
			}
		}
	}

	// ���·����ڴ棨����ʽ��
	return MallocMemory(nNeedMemorySize);	
}

void CMemoryPoolManag::FreeMemory( void* pMemory )
{
	throw std::exception("The method or operation is not implemented.");
}
