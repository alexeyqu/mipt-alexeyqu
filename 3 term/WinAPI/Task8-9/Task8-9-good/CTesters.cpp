#include "CTesters.h"

void dumbTestHeap()
{
	myHeap.Create(SZ_4KB, SZ_128KB);

	LOG("\n\nNEW");
	myHeap.printPageTable();
	myHeap.printMemSets();
	BREAKPOINT;

	C1KB *mem = new C1KB();

	LOG("\n\nDEL");
	myHeap.printPageTable();
	myHeap.printMemSets();
	BREAKPOINT;

	delete(mem);

	LOG("\n\nFIN");
	myHeap.printPageTable();
	myHeap.printMemSets();
	BREAKPOINT;

	myHeap.Destroy();

	myHeap.printPageTable();
	myHeap.printMemSets();
	BREAKPOINT;
}

void simpleTestHeap()
{
	myHeap.Create(SZ_4KB, SZ_32MB);

	for (int i = 0; i < 50; i++)
	{
		C1KB *mem1 = new C1KB();
		CRandomSize *mem3 = new CRandomSize();
		C256KB *mem2 = new C256KB();

		myHeap.printPageTable();
		myHeap.printMemSets();
		myHeap.printMemUsage();

		delete(mem3);
		delete(mem1);

		myHeap.printPageTable();
		myHeap.printMemSets();
		myHeap.printMemUsage();

		// std::cin.get();

		//		delete(mem2);
	}

	myHeap.Destroy();
}

void TimingWinHeap(int steps, int window)
{
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	std::list<C256KB_std*> l;

	winHeap = ::HeapCreate(HEAP_NO_SERIALIZE, SZ_4KB, SZ_32MB);

	for (int i = 0; i < steps; ++i)
	{
		C256KB_std* c = new C256KB_std();
		l.push_back(c);

		if (i >= window)
		{
			int r = rand() % l.size();
			auto it = l.begin();

			for (int j = 0; j < r; j++)
			{
				it++;
			}

			C256KB_std* toDelete = *it;
			l.erase(it);
			delete(toDelete);
		}
	}

	::HeapDestroy(winHeap);

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	auto timeElapsed = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	std::cout << "Standart: time elapsed " << timeElapsed << std::endl;
	BREAKPOINT;
}

void TimingMyHeap(int steps, int window)
{
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	myHeap.Create(SZ_4KB, SZ_32MB);

	std::list<C256KB*> l;

	for (int i = 0; i < steps; ++i)
	{
		C256KB* c = new C256KB();
		l.push_back(c);
		/*
		myHeap.printPageTable();
		myHeap.printMemSets();
		myHeap.printMemUsage();

		std::cin.get();
		*/
		if (i >= window)
		{
			int r = rand() % l.size();
			auto it = l.begin();

			for (int j = 0; j < r; j++)
			{
				it++;
			}

			C256KB* toDelete = *it;
			l.erase(it);
			delete(toDelete);
		}
	}

	myHeap.Destroy();

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	auto timeElapsed = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	std::cout << "MyHeap: time elapsed " << timeElapsed << std::endl;
	BREAKPOINT;
}
