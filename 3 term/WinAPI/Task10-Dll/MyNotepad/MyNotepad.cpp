// MyNotepadDynamic.cpp: ���������� ����� ����� ��� ����������.
//

#include "stdafx.h"
// MyNotepad.cpp: ���������� ����� ����� ��� ����������� ����������.
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
