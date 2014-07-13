#pragma once

//////////////////////////////////////////////////////////////////////////
//
//		内存池管理类
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
	// 根据客户所需要分配的内存大小，计算出要申请的内存以及需要的管理类
	// 分配内存（内存块和管理类），并将原始内存分配到管理类中来计算出实际需要分配的内存
	//
	//////////////////////////////////////////////////////////////////////////
	void* MallocMemory(const size_t& nMallocMemorySize);

	// 将申请到的内存块分配到管理类中
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
	//	根据用户申请的内存大小来分配，如果有足够大的内存则分配，否则再申请更大块的内存
	//  
	//	分配内存必定要是连续的内存块
	//
	/////////////////////////////////////////////////////////////////////////
	virtual void* GetMemory(const size_t& nNeedMemorySize);
	virtual void FreeMemory( void* pMemory );

private:

	// 重新计算每块内存可用的大小
	void ReCalcAllBlockSize();
	
	// 
	MemoryChunk* SkipChunks(MemoryChunk* pChunk, int nSkipChunkCount);
	
	// 检测某大块内存中是否有已经被分配的内存块
	MemoryChunk* IsAllocated( MemoryChunk* pChunk, size_t nNeedBlockCount );

	// 根据数据指针找到对应的管理类
	MemoryChunk* FindMemoryBlockByData( void* pMemory );

private:
	// 内存管理链表集合
	MemoryChunk* m_pFirstChunk;
	MemoryChunk* m_pLastChunk;

	// 所有申请原始内存块大小
	size_t m_nMemoryPoolSize;
	// 所有内存块个数
	size_t m_nMemoryPoolBlockSize;
	// 每块原始内存大小
	size_t m_nOrignBlockSize;	
};

#endif