#pragma once

//////////////////////////////////////////////////////////////////////////
//
//		�ڴ�ع�����
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
	// ���ݿͻ�����Ҫ������ڴ��С�������Ҫ������ڴ��Լ���Ҫ�Ĺ�����
	// �����ڴ棨�ڴ��͹����ࣩ������ԭʼ�ڴ���䵽���������������ʵ����Ҫ������ڴ�
	//
	//////////////////////////////////////////////////////////////////////////
	void* MallocMemory(const size_t& nMallocMemorySize);

	// �����뵽���ڴ����䵽��������
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
	//	�����û�������ڴ��С�����䣬������㹻����ڴ�����䣬����������������ڴ�
	//
	/////////////////////////////////////////////////////////////////////////
	void* GetMemory(const size_t& nNeedMemorySize);
	void FreeMemory( void* pMemory );

private:
	// �ڴ����������
	MemoryChunk* m_pFirstChunk;
	MemoryChunk* m_pCurrentChunk;
	MemoryChunk* m_pLastChunk;


	// ��������ԭʼ�ڴ���С
	size_t m_nMemoryPoolSize;
	// ÿ��ԭʼ�ڴ��С
	size_t m_nOrignBlockSize;	
};