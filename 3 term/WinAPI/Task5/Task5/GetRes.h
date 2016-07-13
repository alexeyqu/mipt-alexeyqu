#pragma once

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <vector>

class COverlappedWindow {
public:
	COverlappedWindow() {};
	~COverlappedWindow() {};

	static bool RegisterClass(HINSTANCE hInstance, const wchar_t CLASS_NAME[]);

	bool Create(HINSTANCE hInstance, const wchar_t CLASS_NAME[]);

	void Show(int cmdShow);

protected:
	void onDestroy();

	void onCreate();

	void onClose();

	void onSize();

	void onCommand(WPARAM wParam, LPARAM lParam);

private:
	HWND handle, handleEdit;

	HACCEL haccel;

	HMENU handleMenu;

	bool textChanged;

	wchar_t* textBuffer;

	static LRESULT __stdcall MsgRouter(HWND handle, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	LRESULT __stdcall wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void storeHandle(HWND _handle);

	void saveText();

	void handlePaint();
};
