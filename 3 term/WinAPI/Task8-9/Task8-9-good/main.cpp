#include "CTesters.h"
#include "CHeapManager.h"

int main(int argc, char *argv[]) // SUBSYSTEM: CONSOLE
// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prev, LPSTR lpCmdLine, int nCmdShow) // SUBSYSTEM: WIN32
{
//	dumbTestHeap();

//	simpleTestHeap();

	TimingWinHeap(10000, 30);

	TimingMyHeap(10000, 30);
	
	std::cin.get();

	return 0;
}