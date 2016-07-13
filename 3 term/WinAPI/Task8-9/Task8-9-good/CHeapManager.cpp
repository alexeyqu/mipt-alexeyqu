#include "CHeapManager.h"

// MAIN METHODS

CHeapManager::CHeapManager() {}

CHeapManager::~CHeapManager() {}

void CHeapManager::Create(size_t minSize, size_t maxSize)
{
	void *ptrReserved = ::VirtualAlloc(NULL, maxSize, MEM_RESERVE, PAGE_READWRITE);
	if (ptrReserved == NULL)
	{
		errorHandler(std::string("VirtualAlloc MEM_RESERVE"), true);
	}

	minSize = ceilToBlockSize(minSize, SZ_PAGESIZE);

	addToMemSet(std::make_pair(ptrReserved, maxSize));
	
	maxHeapSize = maxSize;
	minHeapSize = minSize;
	ptrHeapStart = ptrReserved;

	int gotMemPages = maxSize / SZ_PAGESIZE + ((maxSize % SZ_PAGESIZE == 0) ? 0 : 1);

	LOG(gotMemPages);

	pageTable.resize(gotMemPages);

	BREAKPOINT;
	Alloc(minSize);

	DBG_ACTION printPageTable();
	BREAKPOINT;
}

void *CHeapManager::Alloc(size_t size)
{
	LOG("\nALLOC");

	size = ceilToBlockSize(size, SZ_4B);

	memPtrSize allocated = getMemFromSet(size);
	if (allocated.first == NULL)
	{
		errorHandler("getMemFromSet", true);
	}

	BREAKPOINT;

	addToPageTable(allocated);

	DBG_ACTION printPageTable();
	DBG_ACTION printMemSets();
	BREAKPOINT;

	size_t memSize = allocated.second;
	size_t extraSize = memSize - size;

	void *extraMemPtr = adjustPointer(allocated.first, size);

	delFromPageTable(std::make_pair(extraMemPtr, extraSize));

	addToMemSet(std::make_pair(extraMemPtr, extraSize));

	allocatedMemSet.insert(std::make_pair(allocated.first, size));

	DBG_ACTION printPageTable();
	DBG_ACTION printMemSets();
	BREAKPOINT;
	
	return allocated.first;
}

void CHeapManager::Free(void* memPtr)
{
//	LOG("\nFREE");

	for (auto it = allocatedMemSet.begin(); it != allocatedMemSet.end(); )
	{
		if (it->first == memPtr)
		{
			addToMemSet(*it);

			delFromPageTable(*it);

			Merge(*it);

			allocatedMemSet.erase(it++);
		}
		else
		{
			it++;
		}
	}
}

void CHeapManager::Destroy()
{
	Free(ptrHeapStart);

	if (allocatedMemSet.size() == 0)
	{
		for (auto it = smallMemSet.begin(); it != smallMemSet.end(); it++)
		{
			size_t currentSize = it->second;
			::VirtualFree(it->first, currentSize, MEM_DECOMMIT);
		}

		for (auto it = mediumMemSet.begin(); it != mediumMemSet.end(); it++)
		{
			size_t currentSize = it->second;
			::VirtualFree(it->first, currentSize, MEM_DECOMMIT);
		}

		for (auto it = largeMemSet.begin(); it != largeMemSet.end(); it++)
		{
			size_t currentSize = it->second;
			::VirtualFree(it->first, currentSize, MEM_DECOMMIT);
		}

		::VirtualFree(ptrHeapStart, maxHeapSize, MEM_RELEASE);
	}
	else
	{
		std::cout << "Left allocated:" << std::endl;

		for (auto it = allocatedMemSet.begin(); it != allocatedMemSet.end(); it++)
		{
			std::cout << it->first << " : " << it->second << std::endl;
		}
	}
}

// MISC

void *CHeapManager::adjustPointer(void *memPtr, size_t memSize)
{
	return static_cast<char*> (memPtr) + memSize;
}

size_t CHeapManager::ceilToBlockSize(size_t size, size_t blockSize)
{
	return (size % blockSize == 0) ? size : (size / blockSize + 1) * blockSize;
}