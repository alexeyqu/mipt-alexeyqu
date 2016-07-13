#include "CHeapManager.h"

// MERGING

void CHeapManager::Merge(memPtrSize memBlock)
{
	memPtrSize mergeBlock = memBlock;

	while (1)
	{
		mergeBlock = mergeIteration(mergeBlock);
		if (mergeBlock.first == NULL)
		{
			break;
		}
	}
}

memPtrSize CHeapManager::mergeIteration(memPtrSize mergeBlock)
{
	if (smallMemSet.size() > 0)
	{
		auto smallLowerBound = smallMemSet.lower_bound(mergeBlock);
		if (smallLowerBound != smallMemSet.end() &&
			*smallLowerBound == mergeBlock)
		{
			smallLowerBound--;
		}

		auto smallUpperBound = smallMemSet.upper_bound(mergeBlock);
		if (smallUpperBound == smallMemSet.end())
		{
			smallUpperBound--;
		}

		if (smallLowerBound != smallMemSet.end() &&
			canMerge(*smallLowerBound, mergeBlock) == LMERGE)
		{
			memPtrSize newBlock = mergeBlocks(*smallLowerBound, mergeBlock);
			smallMemSet.erase(smallLowerBound);
			delFromMemSet(mergeBlock);
			addToMemSet(newBlock);
			return newBlock;
		}

		if (canMerge(mergeBlock, *smallUpperBound) == LMERGE)
		{
			memPtrSize newBlock = mergeBlocks(mergeBlock, *smallUpperBound);
			smallMemSet.erase(smallUpperBound);
			delFromMemSet(mergeBlock);
			addToMemSet(newBlock);
			return newBlock;
		}
	}

	if (mediumMemSet.size() > 0)
	{
		auto mediumLowerBound = mediumMemSet.lower_bound(mergeBlock);
		if (mediumLowerBound != mediumMemSet.end() &&
			*mediumLowerBound == mergeBlock)
		{
			mediumLowerBound--;
		}

		auto mediumUpperBound = mediumMemSet.upper_bound(mergeBlock);
		if (mediumUpperBound == mediumMemSet.end())
		{
			mediumUpperBound--;
		}

		if (mediumLowerBound != mediumMemSet.end() &&
			canMerge(*mediumLowerBound, mergeBlock) == LMERGE)
		{
			memPtrSize newBlock = mergeBlocks(*mediumLowerBound, mergeBlock);
			mediumMemSet.erase(mediumLowerBound);
			delFromMemSet(mergeBlock);
			addToMemSet(newBlock);
			return newBlock;
		}

		if (canMerge(mergeBlock, *mediumUpperBound) == LMERGE)
		{
			memPtrSize newBlock = mergeBlocks(mergeBlock, *mediumUpperBound);
			mediumMemSet.erase(mediumUpperBound);
			delFromMemSet(mergeBlock);
			addToMemSet(newBlock);
			return newBlock;
		}
	}

	if (largeMemSet.size() > 0)
	{
		auto largeLowerBound = largeMemSet.lower_bound(mergeBlock);
		if (largeLowerBound != largeMemSet.end() &&
			*largeLowerBound == mergeBlock)
		{
			largeLowerBound--;
		}

		auto largeUpperBound = largeMemSet.upper_bound(mergeBlock);
		if (largeUpperBound == largeMemSet.end())
		{
			largeUpperBound--;
		}

		if (largeLowerBound != largeMemSet.end() &&
			canMerge(*largeLowerBound, mergeBlock) == LMERGE)
		{
			memPtrSize newBlock = mergeBlocks(*largeLowerBound, mergeBlock);
			largeMemSet.erase(largeLowerBound);
			delFromMemSet(mergeBlock);
			addToMemSet(newBlock);
			return newBlock;
		}

		if (canMerge(mergeBlock, *largeUpperBound) == LMERGE)
		{
			memPtrSize newBlock = mergeBlocks(mergeBlock, *largeUpperBound);
			largeMemSet.erase(largeUpperBound);
			delFromMemSet(mergeBlock);
			addToMemSet(newBlock);
			return newBlock;
		}
	}

	return (std::make_pair((void *)NULL, -1));
}

MERGEWAYS CHeapManager::canMerge(memPtrSize mergeBlock1, memPtrSize mergeBlock2)
{
	if (adjustPointer(mergeBlock1.first, mergeBlock1.second) == mergeBlock2.first) return LMERGE;
	if (adjustPointer(mergeBlock2.first, mergeBlock2.second) == mergeBlock1.first) return RMERGE;

	return NOMERGE;
}

memPtrSize CHeapManager::mergeBlocks(memPtrSize memBlock1, memPtrSize memBlock2)
{
	memPtrSize newBlock = std::make_pair(memBlock1.first, (memBlock1.second + memBlock2.second));

	return newBlock;
}
