#include "Ellipse.h"

int WINAPI wWinMain(HINSTANCE hInstance, 
	HINSTANCE hPrevInstance,
	LPWSTR szCmdLine, 
	int cmdShow)
{
	const wchar_t className[] = L"myPrettyClass";

	COverlappedWindow myWindow;

	myWindow.RegisterClass(hInstance, className);

	myWindow.Create(hInstance, className);

	myWindow.Show(cmdShow);

	return 0;
}