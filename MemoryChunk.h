#pragma once

//////////////////////////////////////////////////////////////////////////
//
//		�����ڴ��������Ϣ
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
		m_bIsBlockHeader = false;
		m_bIsAllocate = false;
	}

	void Initial() 
	{
		m_nSize = 0;
		m_nUsed = 0;
		m_pData = NULL;
		m_pNext = NULL;
		m_bIsBlockHeader = false;
		m_bIsAllocate = false;
	}


	// ���ݿ��С
	size_t			m_nSize;
	// ʹ�ô�С
	size_t			m_nUsed;
	// ���ݿ�
	BYTE*			m_pData;	
	// �Ƿ������ݿ�ͷָ�루һ�����ڴ棩
	bool			m_bIsBlockHeader;
	// �Ƿ�����
	bool			m_bIsAllocate;
	// ��һ������
	MemoryChunk*	m_pNext;
};

