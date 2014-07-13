#pragma once

//////////////////////////////////////////////////////////////////////////
//
//		�ڴ�ع�����
//
//////////////////////////////////////////////////////////////////////////

#ifndef _MEMORY_POOL_MANAG_H_
#define _MEMORY_POOL_MANAG_H_


#include "IMemoryPool.h"
#include <math.h>

using namespace std;
typedef unsigned char BYTE;

class CMemoryPoolManag : public IMemoryPool
{
public:
	CMemoryPoolManag(const size_t& nMemoryPoolSize, const size_t& nOrignBlockSize);
	virtual ~CMemoryPoolManag(void);

private:
	void Initial(const size_t& nMemoryPoolSize);
	void Release();

private:
	//////////////////////////////////////////////////////////////////////////
	//
	// ���ݿͻ�����Ҫ������ڴ��С�������Ҫ������ڴ��Լ���Ҫ�Ĺ�����
	// �����ڴ棨�ڴ��͹����ࣩ������ԭʼ�ڴ���䵽���������������ʵ����Ҫ������ڴ�
	//
	//////////////////////////////////////////////////////////////////////////
	void* MallocMemory(const size_t& nMallocMemorySize);

	// �����뵽���ڴ����䵽��������
	MemoryChunk* DistributeMemoryToChunk( BYTE* pOrignMemory, const size_t& nBlockCount, const size_t& nTotalMallocMemorySize );


private:
	// util method
	size_t GetBlockCount(const size_t& nMemorySize)//inline function
	{		
		return (size_t)ceil((double)nMemorySize/m_nOrignBlockSize);
	}

public:
	//////////////////////////////////////////////////////////////////////////
	//
	//	�����û�������ڴ��С�����䣬������㹻����ڴ�����䣬����������������ڴ�
	//  
	//	�����ڴ�ض�Ҫ���������ڴ��
	//
	/////////////////////////////////////////////////////////////////////////
	virtual void* GetMemory(const size_t& nNeedMemorySize);
	virtual void FreeMemory( void* pMemory );

private:

	// ���¼���ÿ���ڴ���õĴ�С
	void ReCalcAllBlockSize();
	
	// 
	MemoryChunk* SkipChunks(MemoryChunk* pChunk, int nSkipChunkCount);
	
	// ���ĳ����ڴ����Ƿ����Ѿ���������ڴ��
	MemoryChunk* IsAllocated( MemoryChunk* pChunk, size_t nNeedBlockCount );

	// ��������ָ���ҵ���Ӧ�Ĺ�����
	MemoryChunk* FindMemoryBlockByData( void* pMemory );

private:
	// �ڴ����������
	MemoryChunk* m_pFirstChunk;
	MemoryChunk* m_pLastChunk;

	// ��������ԭʼ�ڴ���С
	size_t m_nMemoryPoolSize;
	// �����ڴ�����
	size_t m_nMemoryPoolBlockSize;
	// ÿ��ԭʼ�ڴ��С
	size_t m_nOrignBlockSize;	
};

#endif