#include "EditControl.h"
#include "resource.h"

int getCurrentFontSize(HWND editControlHandle)
{
	HFONT curFont = (HFONT) ::SendMessage(editControlHandle, WM_GETFONT, 0, 0);
	LOGFONT fontParams;
	::GetObject(curFont, sizeof(LOGFONT), &fontParams);
	return fontParams.lfHeight;
}

void setFontSize(HWND editControlHandle, int fontSize)
{
	HFONT curFont = (HFONT) ::SendMessage(editControlHandle, WM_GETFONT, 0, 0);

	if (!curFont) {
		curFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	}

	LOGFONT fontParams;
	::GetObject(curFont, sizeof(LOGFONT), &fontParams);
	fontParams.lfHeight = fontSize;

	curFont = ::CreateFontIndirect(&fontParams);

	::SendMessage(editControlHandle, WM_SETFONT, (WPARAM)curFont, 1);
}

void setAlphaChannel(HWND handle, int alpha)
{
	DWORD curStyle = GetWindowLong(handle, GWL_EXSTYLE) | WS_EX_LAYERED;

	SetWindowLong(handle, GWL_EXSTYLE, curStyle);

	::SetLayeredWindowAttributes(handle, 0, alpha, LWA_ALPHA);
}

void COverlappedWindow::viewSettings(WPARAM wParam, LPARAM lParam)
{
	dialogHandle = ::CreateDialog(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), handle, dialogMsgRouter);

	if (dialogHandle == NULL)
	{
		::MessageBox(NULL, L"ERROR CreateDialog", L"ERROR", MB_ICONERROR);
		return;
	}

	::ShowWindow(dialogHandle, SW_SHOW);
}

BOOL __stdcall COverlappedWindow::dialogMsgRouter(HWND dialogHandle, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	COverlappedWindow *thisWindow = (COverlappedWindow *) ::GetWindowLongPtr(::GetParent(dialogHandle), GWL_USERDATA);

	thisWindow->dialogHandle = dialogHandle;

	return thisWindow->dialogProc(uMsg, wParam, lParam);
}

BOOL __stdcall COverlappedWindow::dialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		onDialogInit();
		return true;

	case WM_HSCROLL:
		onDialogSlider();
		return true;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
			onDialogOK();
			return true;

		case IDCANCEL:
			onDialogCancel();
			return true;

		case IDC_BUTTON1:
		case IDC_BUTTON2:
			onButtonColor(LOWORD(wParam));
			return true;

		case IDC_CHECK1:
			onDialogCheck();
			return true;
		}
	}
	}

	return false;
}

void COverlappedWindow::onDialogInit()
{
	HWND alphaSlider = ::GetDlgItem(dialogHandle, IDC_SLIDERALPHA);
	HWND fontSizeSlider = ::GetDlgItem(dialogHandle, IDC_SLIDERFONT);
	HWND previewCheckBox = ::GetDlgItem(dialogHandle, IDC_CHECK1);

	if (dialogHandle == NULL)
	{
		::MessageBox(NULL, L"ERROR GetDlgItems", L"ERROR", MB_ICONERROR);
		return;
	}

	::SendMessage(alphaSlider, TBM_SETRANGE, 1, MAKELONG(MINALPHA, MAXALPHA));
	::SendMessage(fontSizeSlider, TBM_SETRANGE, 1, MAKELONG(MINFONT, MAXFONT));

	current.fontSize = ::getCurrentFontSize(handleEdit);

	::SendMessage(fontSizeSlider, TBM_SETPOS, 1, current.fontSize);
	::SendMessage(alphaSlider, TBM_SETPOS, 1, current.alpha);

	if (isPreviewChecked)
		::SendMessage(previewCheckBox, BM_SETCHECK, BST_CHECKED, 0);
	else 
		::SendMessage(previewCheckBox, BM_SETCHECK, BST_UNCHECKED, 0);
}

void COverlappedWindow::onDialogSlider()
{
	HWND alphaSlider = ::GetDlgItem(dialogHandle, IDC_SLIDERALPHA);
	HWND fontSizeSlider = ::GetDlgItem(dialogHandle, IDC_SLIDERFONT);

	int curPosAlpha = ::SendMessage(alphaSlider, TBM_GETPOS, 0, 0);
	int curPosFontSize = ::SendMessage(fontSizeSlider, TBM_GETPOS, 0, 0);

	tmp.alpha = curPosAlpha;
	tmp.fontSize = curPosFontSize;

	if (isPreviewChecked)
	{
		setAlphaChannel(handle, tmp.alpha);
		setFontSize(handleEdit, tmp.fontSize);
		InvalidateRect(handleEdit, NULL, 1);
		InvalidateRect(handle, NULL, 1);
	}
}

void COverlappedWindow::onDialogOK()
{
	current.alpha = tmp.alpha;
	current.backgroundColor = tmp.backgroundColor;
	current.fontColor = tmp.fontColor;
	current.fontSize = tmp.fontSize;

	setAlphaChannel(handle, current.alpha);
	setFontSize(handleEdit, current.fontSize);

	::DestroyWindow(dialogHandle);
}

void COverlappedWindow::onDialogCancel()
{
	tmp.alpha = current.alpha;
	tmp.backgroundColor = current.backgroundColor;
	tmp.fontColor = current.fontColor;
	tmp.fontSize = current.fontSize;

	setAlphaChannel(handle, tmp.alpha);
	setFontSize(handleEdit, tmp.fontSize);

	::InvalidateRect(handle, NULL, 1);
	::InvalidateRect(handleEdit, NULL, 1);
	::DestroyWindow(dialogHandle);
}

void COverlappedWindow::onDialogCheck()
{
	int currentPosFontSize;
	int currentPosAlpha;

	HWND previewCheckBox = ::GetDlgItem(dialogHandle, IDC_CHECK1);
	isPreviewChecked = ::SendMessage(previewCheckBox, BM_GETCHECK, 0, 0);

	if (isPreviewChecked)
	{
		currentPosFontSize = tmp.fontSize;
		currentPosAlpha = tmp.alpha;
	}
	else
	{
		currentPosFontSize = current.fontSize;
		currentPosAlpha = current.alpha;
	}

	setFontSize(handleEdit, currentPosFontSize);
	setAlphaChannel(handle, currentPosAlpha);
	::InvalidateRect(handle, NULL, 1);
	::InvalidateRect(handleEdit, NULL, 1);
}

void COverlappedWindow::onButtonColor(UINT buttonID)
{
	CHOOSECOLOR colors;
	static COLORREF myColors[16];

	memset(&colors, 0, sizeof(colors));
	colors.lStructSize = sizeof(colors);
	colors.hwndOwner = dialogHandle;
	colors.lpCustColors = (LPDWORD)myColors;
	colors.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (::ChooseColor(&colors) == true)
	{
		if (buttonID == IDC_BUTTON2)
		{
			tmp.fontColor = colors.rgbResult;
		}
		if (buttonID == IDC_BUTTON1)
		{
			tmp.backgroundColor = colors.rgbResult;
		}
	}

	if (isPreviewChecked)
	{
		::InvalidateRect(handle, NULL, 1);
		::InvalidateRect(handleEdit, NULL, 1);
	}
}