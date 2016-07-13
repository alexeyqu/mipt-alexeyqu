// MyNotepadDynamic.cpp: определ€ет точку входа дл€ приложени€.
//

#include "stdafx.h"
// MyNotepad.cpp: определ€ет точку входа дл€ консольного приложени€.
//
#include "EditControl.h"

int WINAPI wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR szCmdLine,
	int cmdShow)
{
	const wchar_t className[] = L"myPrettyClassWithEditChild";

	COverlappedWindow myWindow;

	myWindow.RegisterClass(hInstance, className);

	myWindow.Create(hInstance, className);

	myWindow.Show(cmdShow);

	return 0;
}
