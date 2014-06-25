#pragma once

//////////////////////////////////////////////////////////////////////////
//
//		内存池管理类
//
//////////////////////////////////////////////////////////////////////////

using namespace std;
typedef unsigned char BYTE;

struct MemoryChunk;
class CMemoryPoolManag
{
public:
	CMemoryPoolManag(const size_t& nMemoryPoolSize, const size_t& nOrignBlockSize);
	~CMemoryPoolManag(void);

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
	void DistributeMemoryToChunk( BYTE* pOrignMemory, const size_t& nBlockCount, const size_t& nTotalMallocMemorySize );


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
	/////////////////////////////////////////////////////////////////////////
	void* GetMemory(const size_t& nNeedMemorySize);
	void FreeMemory( void* pMemory );

private:
	// 内存管理链表集合
	MemoryChunk* m_pFirstChunk;
	MemoryChunk* m_pCurrentChunk;
	MemoryChunk* m_pLastChunk;


	// 所有申请原始内存块大小
	size_t m_nMemoryPoolSize;
	// 每块原始内存大小
	size_t m_nOrignBlockSize;	
};