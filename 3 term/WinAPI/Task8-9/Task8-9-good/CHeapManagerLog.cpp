#include "CHeapManager.h"

// LOGGING

void CHeapManager::errorHandler(std::string errorMsg, bool critical)
{
	std::cout << "ERROR: " << errorMsg << std::endl;
	std::cin.get();
	if (critical)
	{
		exit(1);
	}
}

void CHeapManager::printPageTable()
{
	std::cout << "PageTable\n";
	for (size_t i = 0; i < pageTable.size(); i++)
	{
		std::cout << pageTable[i] << ' ';
	}
	std::cout << std::endl;
}

void CHeapManager::printMemSets()
{
	std::cout << "\nMEMSETS\n";

	std::cout << "small\n";

	for (auto it = smallMemSet.begin(); it != smallMemSet.end(); it++)
	{
		std::cout << it->first << " = " << it->second << std::endl;
	}

	std::cout << "medium\n";

	for (auto it = mediumMemSet.begin(); it != mediumMemSet.end(); it++)
	{
		std::cout << it->first << " = " << it->second << std::endl;
	}

	std::cout << "large\n";

	for (auto it = largeMemSet.begin(); it != largeMemSet.end(); it++)
	{
		std::cout << it->first << " = " << it->second << std::endl;
	}

	std::cout << "allocated\n";

	for (auto it = allocatedMemSet.begin(); it != allocatedMemSet.end(); it++)
	{
		std::cout << it->first << " = " << it->second << std::endl;
	}

	std::cout << std::endl;
}

size_t CHeapManager::getAllocatedSize()
{
	size_t result = 0;
	for (auto it = allocatedMemSet.begin(); it != allocatedMemSet.end(); it++)
	{
		result += it->second;
	}

	LOG("Allocated total: ");
	LOG(result);

	return result;
}

size_t CHeapManager::getCommitedSize()
{
	size_t result = 0;
	for (size_t i = 0; i < pageTable.size(); i++)
	{
		if (pageTable[i] != 0)
		{
			result += SZ_PAGESIZE;
		}
	}

	LOG("Commited total: ");
	LOG(result);

	return result;
}

void CHeapManager::printMemUsage()
{
	size_t alloc = getAllocatedSize();
	size_t commt = getCommitedSize();

	std::cout << "Usage: " << (double)alloc / commt << std::endl;
	std::cout << "Total heap size: " << maxHeapSize << std::endl;
}