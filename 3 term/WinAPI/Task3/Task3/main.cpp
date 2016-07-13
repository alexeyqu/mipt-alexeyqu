#include "ParentWindow.h"

int WINAPI wWinMain(HINSTANCE hInstance, 
	HINSTANCE hPrevInstance,
	LPWSTR szCmdLine, 
	int cmdShow)
{
	const wchar_t className[] = L"myPrettyParentClass";

	COverlappedWindow myWindow;

	myWindow.RegisterClass(hInstance, className);

	myWindow.Create(hInstance, className);

	myWindow.Show(cmdShow);

	return 0;
}