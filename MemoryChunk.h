#pragma once

//////////////////////////////////////////////////////////////////////////
//
//		各个内存管理类信息
//
//////////////////////////////////////////////////////////////////////////

//typedef BYTE unsigned char;

struct MemoryChunk
{
	MemoryChunk::MemoryChunk()
	{
		m_nSize = 0;
		m_nUsed = 0;
		m_pData = NULL;
		m_pNext = NULL;
		m_bIsAllocate = false;
	}

	void Initial() 
	{
		m_nSize = 0;
		m_nUsed = 0;
		m_pData = NULL;
		m_pNext = NULL;
		m_bIsAllocate = false;
	}


	// 数据块大小
	size_t			m_nSize;
	// 使用大小
	size_t			m_nUsed;
	// 数据块
	BYTE*			m_pData;	
	// 是否被申请
	bool			m_bIsAllocate;
	// 下一块数据
	MemoryChunk*	m_pNext;
};

