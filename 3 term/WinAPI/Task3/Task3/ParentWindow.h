#pragma once

#include <windows.h>
#include <stdio.h>

#include "EllipseWindow.h"

class COverlappedWindow {
public:
	COverlappedWindow() {};
	~COverlappedWindow() {};

	static bool RegisterClass(HINSTANCE hInstance, const wchar_t CLASS_NAME[]);

	bool Create(HINSTANCE hInstance, const wchar_t CLASS_NAME[]);

	void Show(int cmdShow);

protected:
	void OnDestroy();

private:
	HWND handle;

	CEllipseWindow topLeftChild, topRightChild, bottomLeftChild, bottomRightChild;

	static LRESULT __stdcall MsgRouter(HWND handle, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	LRESULT __stdcall wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void storeHandle(HWND _handle);

	void onClose();

	void onSize();

	void onCreate();

	void handlePaint();
};
