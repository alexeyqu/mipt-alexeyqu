#pragma once

#include "CHeapManager.h"
#include <chrono>
#include <list>

static CHeapManager myHeap;
static HANDLE winHeap;

class C1KB
{
public:
	void* operator new(size_t size)
	{
		return myHeap.Alloc(SZ_1KB);
	}
	void operator delete(void* ptr)
	{
		myHeap.Free(ptr);
	}
};

class C64KB
{
public:	
	void* operator new(size_t size)
	{
		return myHeap.Alloc(SZ_64KB);
	}
	void operator delete(void* ptr)
	{
		myHeap.Free(ptr);
	}
};

class C256KB
{
public:
	void* operator new(size_t size)
	{
		return myHeap.Alloc(SZ_256KB);
	}
		void operator delete(void* ptr)
	{
		myHeap.Free(ptr);
	}
};

class C256KB_std
{
//	char std[SZ_256KB];

public:
	void* operator new(size_t size)
	{
		return ::HeapAlloc(winHeap, HEAP_ZERO_MEMORY, SZ_256KB);
	}
		void operator delete(void* ptr)
	{
		::HeapFree(winHeap, NULL, ptr);
	}
};

class C4MB
{
public:
	void* operator new(size_t size)
	{
		return myHeap.Alloc(SZ_1MB * 4);
	}
		void operator delete(void* ptr)
	{
		myHeap.Free(ptr);
	}
};

class CRandomSize
{
public:
	void* operator new(size_t size)
	{
		size = std::rand() % myHeap.getMaxHeapSize() + 1;

		std::cout << "Random size: " << size << std::endl;
		//BREAKPOINT;

		return myHeap.Alloc(size);
	}
	void operator delete(void* ptr)
	{
		myHeap.Free(ptr);
	}
};

void dumbTestHeap();

void simpleTestHeap();

void TimingWinHeap(int steps, int window);
void TimingMyHeap(int steps, int window);
