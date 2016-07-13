#pragma once

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <tchar.h>
#include <string>
#include <sstream>
#include <iterator>

#include <stdio.h>
#include <wtypes.h>

#define MAX_TEXT_SIZE 100
#define N_OF_PROCESSES 4

class CMyNotepad 
{
public:
	CMyNotepad() {};
	~CMyNotepad() {};

	static bool RegisterClass(HINSTANCE hInstance, const wchar_t CLASS_NAME[]);

	bool Create(HINSTANCE hInstance, const wchar_t CLASS_NAME[]);

	void Show(int cmdShow);

	int myOutputDebugPrintf(const wchar_t format[], ...);

protected:
	void onDestroy();

	void onCreate();

	void onClose();

	void onSize();

	void onClear();

	void onCommand(WPARAM wParam, LPARAM lParam);

private:
	HWND handle, handleEdit;

	HACCEL haccel;

	bool textChanged;

	wchar_t* textBuffer;

	static LRESULT __stdcall MsgRouter(HWND handle, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT __stdcall wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void storeHandle(HWND _handle);

	void saveText();

	void openFile();

	void handlePaint();

	PROCESS_INFORMATION procsInfo[N_OF_PROCESSES];
	LPCWSTR mappedFileNames[N_OF_PROCESSES];
	HANDLE mappedFiles[N_OF_PROCESSES];
	HANDLE eventsTextReady[N_OF_PROCESSES];
	HANDLE eventsTextClear[N_OF_PROCESSES];
	HANDLE eventTerminate;

	void procsInit();
};
