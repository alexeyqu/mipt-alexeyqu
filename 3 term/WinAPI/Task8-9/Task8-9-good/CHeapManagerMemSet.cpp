#include "CHeapManager.h"

// MEMSETS JOURNALING

void CHeapManager::addToMemSet(memPtrSize memBlock)
{
	if (memBlock.second <= SZ_SMALL_BORDER)
	{
		smallMemSet.insert(memBlock);
	}
	else if (memBlock.second <= SZ_MEDIUM_BORDER)
	{
		mediumMemSet.insert(memBlock);
	}
	else
	{
		largeMemSet.insert(memBlock);
	}
}

void CHeapManager::delFromMemSet(memPtrSize memBlock)
{
	if (memBlock.second <= SZ_SMALL_BORDER)
	{
		auto it = smallMemSet.find(memBlock);
		if (it != smallMemSet.end())
		{
			smallMemSet.erase(it);
			return;
		}
	}
	else if (memBlock.second <= SZ_MEDIUM_BORDER)
	{
		auto it = mediumMemSet.find(memBlock);
		if (it != mediumMemSet.end())
		{
			mediumMemSet.erase(it);
			return;
		}
	}
	else
	{
		auto it = mediumMemSet.find(memBlock);
		if (it != mediumMemSet.end())
		{
			mediumMemSet.erase(it);
			return;
		}
	}
}

memPtrSize CHeapManager::getMemFromSet(size_t size)
{
	size_t smallSetSize = smallMemSet.size();
	size_t mediumSetSize = mediumMemSet.size();
	size_t largeSetSize = largeMemSet.size();

	memPtrSize allocated = std::make_pair((void *)NULL, -1);

	LOG(smallSetSize);
	LOG(mediumSetSize);
	LOG(largeSetSize);
	LOG(size);

	if (size <= SZ_SMALL_BORDER)
	{
		LOG("SMALL SET");
		if (smallSetSize > 0)
		{
			for (auto it = smallMemSet.begin(); it != smallMemSet.end(); it++)
			{
				if (it->second >= size)
				{
					allocated = *it;
					smallMemSet.erase(it);
					LOG("small");
					return allocated;
				}
			}
		}

		if (mediumSetSize > 0)
		{
			for (auto it = mediumMemSet.begin(); it != mediumMemSet.end(); it++)
			{
				if (it->second >= size)
				{
					allocated = *it;
					mediumMemSet.erase(it);
					LOG("medium");
					return allocated;
				}
			}
		}

		if (largeSetSize > 0)
		{
			for (auto it = largeMemSet.begin(); it != largeMemSet.end(); it++)
			{
				if (it->second >= size)
				{
					allocated = *it;
					largeMemSet.erase(it);
					LOG("large");
					return allocated;
				}
			}
		}
	}

	if (size <= SZ_MEDIUM_BORDER)
	{
		LOG("MEDIUM SET");
		if (mediumSetSize > 0)
		{
			for (auto it = mediumMemSet.begin(); it != mediumMemSet.end(); it++)
			{
				if (it->second >= size)
				{
					allocated = *it;
					mediumMemSet.erase(it);
					LOG("medium");
					return allocated;
				}
			}
		}

		if (largeSetSize > 0)
		{
			for (auto it = largeMemSet.begin(); it != largeMemSet.end(); it++)
			{
				if (it->second >= size)
				{
					allocated = *it;
					largeMemSet.erase(it);
					LOG("large");
					return allocated;
				}
			}
		}
	}

	if (size <= maxHeapSize)
	{
		LOG("LARGE SET");
		if (largeSetSize > 0)
		{
			for (auto it = largeMemSet.begin(); it != largeMemSet.end(); it++)
			{
				if (it->second >= size)
				{
					allocated = *it;
					largeMemSet.erase(it);
					LOG("large");
					return allocated;
				}
			}
		}
	}

	errorHandler(std::string("getMemFromSet mem not found"), true);
	return allocated;
}
