// MemoryPool.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MemoryPoolManag.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CMemoryPoolManag* pMemoryPool = new CMemoryPoolManag(65, 32);
	assert(pMemoryPool != NULL);
	// 申请内存，
	MemoryChunk* pChunk = (MemoryChunk*)pMemoryPool->GetMemory(10);
	void* p1 = pMemoryPool->GetMemory(50);
	void* p2 = pMemoryPool->GetMemory(100);

	// 释放内存
	pMemoryPool->FreeMemory(pChunk);

	//
	delete pMemoryPool;


	system("pause");
	return 0;
}

