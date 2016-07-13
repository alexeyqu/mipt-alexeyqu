#pragma once

#include <windows.h>
#include <string>
#include <iostream>
#include <set>
#include <vector>
#include <cstdlib>

#define SZ_4B 4

#define SZ_1KB 1024

#define SZ_4KB (4 * SZ_1KB)

#define SZ_64KB (64 * SZ_1KB)

#define SZ_128KB (128 * SZ_1KB)

#define SZ_256KB (256 * SZ_1KB)

#define SZ_1MB (1024 * SZ_1KB)

#define SZ_32MB (32 * SZ_1MB)

#define SZ_BLOCK SZ_4B

#define SZ_PAGESIZE SZ_4KB

#define SZ_SMALL_BORDER SZ_4KB

#define SZ_MEDIUM_BORDER SZ_128KB

//#define DEBUG

#ifdef DEBUG
	#define LOG( msg ) \
	{ \
		std::cerr << msg << std::endl; \
	}

	#define DBG_ACTION if(true)

	#define BREAKPOINT \
	{ \
		std::cerr << __FILE__ << " \t: " << __FUNCDNAME__ << " \t: " << __LINE__ << std::endl; \
		std::cin.get(); \
	}
#else
	#define LOG( msg ) if(false) \
	{ \
		std::cout << __FUNCDNAME__ << '\t' << msg << std::endl; \
	}

	#define DBG_ACTION if(false)

	#define BREAKPOINT if(false) \
	{ \
		std::cout << __FILE__ << " \t: " << __FUNCDNAME__ << " \t: " << __LINE__ << std::endl; \
		std::cin.get(); \
	}
#endif

typedef std::pair < void*, size_t > memPtrSize;

enum MERGEWAYS
{
	NOMERGE,
	LMERGE,
	RMERGE
};

class CHeapManager
{
private:
	size_t maxHeapSize, minHeapSize;
	void* ptrHeapStart;

	std::set<memPtrSize> smallMemSet;
	std::set<memPtrSize> mediumMemSet;
	std::set<memPtrSize> largeMemSet;
	std::set<memPtrSize> allocatedMemSet;

	void addToMemSet(memPtrSize memBlock);
	void delFromMemSet(memPtrSize memBlock);
	memPtrSize getMemFromSet(size_t size);

	std::vector<int> pageTable;

	void addToPageTable(memPtrSize memBlock);
	void delFromPageTable(memPtrSize memBlock);

	void Merge(memPtrSize memBlock);
	memPtrSize mergeIteration(memPtrSize mergeBlock);
	MERGEWAYS canMerge(memPtrSize mergeBlock1, memPtrSize mergeBlock2);
	memPtrSize mergeBlocks(memPtrSize memBlock1, memPtrSize memBlock2);

	size_t ceilToBlockSize(size_t size, size_t blockSize);
	void *adjustPointer(void *memPtr, size_t memSize);

	void errorHandler(std::string errorMsg, bool critical);
	size_t getAllocatedSize();
	size_t getCommitedSize();

public:
	CHeapManager();
	~CHeapManager();

	void Create(size_t minSize, size_t maxSize);
	void* Alloc(size_t size);
	void Free(void* memPtr);
	void Destroy();

	size_t getMaxHeapSize() { return maxHeapSize; }

	void printPageTable();
	void printMemSets();
	void printMemUsage();
};