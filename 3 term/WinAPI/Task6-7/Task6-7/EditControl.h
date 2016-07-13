#pragma once

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <Commctrl.h>

const int MINFONT = 8;
const int MAXFONT = 72;

const int MINALPHA = 0;
const int MAXALPHA = 255;

const int DEFAULTBACKGROUNDCOLOR = RGB(255, 255, 255);
const int DEFAULTFONTCOLOR = RGB(0, 0, 0);

const int DEFAULTALPHA = 255;
const int DEFAULTFONTSIZE = 20;

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

	HWND dialogHandle;

	HACCEL haccel;

	HBRUSH backgroundBrush;	

	bool textChanged;

	wchar_t* textBuffer;

	static LRESULT __stdcall MsgRouter(HWND handle, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT __stdcall wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void storeHandle(HWND _handle);

	LRESULT onColorEdit(WPARAM wParam);

	void saveText();

	void viewSettings(WPARAM wParam, LPARAM lParam);

	void handlePaint();

	static BOOL __stdcall dialogMsgRouter(HWND dialogHandle, UINT uMsg, WPARAM wParam, LPARAM lParam);

	BOOL __stdcall dialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void onDialogInit();

	void onDialogSlider();

	void onDialogOK();

	void onDialogCancel();

	void onDialogCheck();

	void onButtonColor(UINT buttonID);

	struct windowProps
	{
		COLORREF backgroundColor = DEFAULTBACKGROUNDCOLOR;
		COLORREF fontColor = DEFAULTFONTCOLOR;
		BYTE alpha = DEFAULTALPHA;
		UINT fontSize = DEFAULTFONTSIZE;
	} current, tmp;

	bool isPreviewChecked = false;
};

int getCurrentFontSize(HWND editControlHandle);

void setFontSize(HWND editControlHandle, int fontSize);

void setAlphaChannel(HWND handle, int alpha);