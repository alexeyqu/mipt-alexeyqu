#include "CHeapManager.h"

// PAGE TABLE JOURNALING

void CHeapManager::addToPageTable(memPtrSize memBlock)
{
	size_t offset = (size_t)memBlock.first - (size_t)ptrHeapStart;

	int startPage = (offset) / SZ_PAGESIZE;
	int endPage = (offset + memBlock.second) / SZ_PAGESIZE + ((offset + memBlock.second) % SZ_PAGESIZE == 0 ? 0 : 1);

	DBG_ACTION std::cout << "ADD\n" << offset << ' ' << memBlock.second << std::endl;
	DBG_ACTION std::cout << "PageStart: " << startPage << ", Pageend: " << endPage << std::endl;
	BREAKPOINT;

	for (int page = startPage; page < endPage; page++)
	{
		if (pageTable[page] == 0)
		{
			void *ptrCommited = ::VirtualAlloc(adjustPointer(ptrHeapStart, page * SZ_PAGESIZE), SZ_PAGESIZE, MEM_COMMIT, PAGE_READWRITE);
			if (ptrCommited == NULL)
			{
				errorHandler("VirtualAlloc addToPageTable failed", true);
			}
		}

		pageTable[page]++;
	}
}

void CHeapManager::delFromPageTable(memPtrSize memBlock)
{
	size_t offset = (size_t)memBlock.first - (size_t)ptrHeapStart;

	int startPage = (offset) / SZ_PAGESIZE + (offset % SZ_PAGESIZE == 0 ? 0 : 1);
	int endPage = (offset + memBlock.second) / SZ_PAGESIZE + ((offset + memBlock.second) % SZ_PAGESIZE == 0 ? 0 : 1);

	DBG_ACTION std::cout << "DEL\n" << offset << ' ' << memBlock.second << std::endl;
	DBG_ACTION std::cout << "PageStart: " << startPage << ", Pageend: " << endPage << std::endl;
	BREAKPOINT;

	for (int page = startPage; page < endPage; page++)
	{
		pageTable[page]--;

		if (pageTable[page] == 0)
		{
			::VirtualFree(adjustPointer(ptrHeapStart, page * SZ_PAGESIZE), SZ_PAGESIZE, MEM_DECOMMIT);
		}
	}
}
