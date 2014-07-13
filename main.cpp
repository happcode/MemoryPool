// MemoryPool.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "MemoryPoolManag.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CMemoryPoolManag* pMemoryPool = new CMemoryPoolManag(96, 32);
	assert(pMemoryPool != NULL);
	// �����ڴ棬
	MemoryChunk* pChunk = (MemoryChunk*)pMemoryPool->GetMemory(10);
	pChunk->m_nSize = 10;
	pChunk->m_nUsed = 10;
	char* p1 = (char*)pMemoryPool->GetMemory(30);
	memcpy(p1, "aaaaaaabbbbbbbbbbbbbbbbbbbbaaaaaaaa", 30);
	cout << "p1 value : " << p1 << endl;
	char* p2 = (char*)pMemoryPool->GetMemory(100);
	memcpy(p2, "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb", 100);
	cout << "p2 value : " << p2 << endl;

	// �ͷ��ڴ�
	pMemoryPool->FreeMemory(p1);

	// ��ȡ�ڴ�
	void* p3 = pMemoryPool->GetMemory(100);
	pMemoryPool->FreeMemory(p3);
	p3 = pMemoryPool->GetMemory(100);

	//
	delete pMemoryPool;


	system("pause");
	return 0;
}

