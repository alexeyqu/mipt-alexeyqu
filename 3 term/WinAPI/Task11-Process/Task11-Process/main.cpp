#include "MyNotepad.h"

int WINAPI wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR szCmdLine,
	int cmdShow)
{
	const wchar_t className[] = L"myNotepadWindowClass";

	CMyNotepad myNotepad;

	myNotepad.RegisterClass(hInstance, className);

	myNotepad.Create(hInstance, className);

	myNotepad.Show(cmdShow);

	return 0;
}