// MemoryPool.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "MemoryPoolManag.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CMemoryPoolManag* pMemoryPool = new CMemoryPoolManag(65, 32);
	assert(pMemoryPool != NULL);
	// �����ڴ棬
	MemoryChunk* pChunk = (MemoryChunk*)pMemoryPool->GetMemory(10);
	void* p1 = pMemoryPool->GetMemory(50);
	void* p2 = pMemoryPool->GetMemory(100);

	// �ͷ��ڴ�
	pMemoryPool->FreeMemory(pChunk);

	//
	delete pMemoryPool;


	system("pause");
	return 0;
}

