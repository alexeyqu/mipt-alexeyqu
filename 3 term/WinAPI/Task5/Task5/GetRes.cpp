#include "GetRes.h"
#include "resource.h"

bool COverlappedWindow::RegisterClass(HINSTANCE hInstance, const wchar_t className[])
{
	WNDCLASSEX wndClass = {};

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = MsgRouter;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = (HICON) ::LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	wndClass.hIconSm = (HICON) ::LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	wndClass.hCursor = ::LoadCursor(NULL,(LPTSTR)IDC_ARROW);
	wndClass.hbrBackground =(HBRUSH) ::GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wndClass.lpszClassName = className;

	const ATOM atom = ::RegisterClassEx(&wndClass);

	if(atom == NULL)
	{
		::MessageBox(NULL, L"ERROR RegisterClassEx", L"Error", MB_ICONWARNING);
		return FALSE;
	}

	return TRUE;
}

bool COverlappedWindow::Create(HINSTANCE hInstance, const wchar_t className[])
{
	const int bufSize = 100;
	wchar_t stringBuffer[bufSize];

	if (!::LoadString(hInstance, IDS_STRING104, stringBuffer, bufSize))
	{
		::MessageBox(NULL, L"ERROR LoadString", L"Error", MB_ICONERROR);
	}

	handle = ::CreateWindowEx(
		0,			// Optional window styles	
		className,	// Window class
		stringBuffer,	 // Window text
		WS_OVERLAPPEDWINDOW,	// Window style
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,	// Size and position
		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		this        // Additional application data
		);

	if(handle == NULL)
	{
		::MessageBox(NULL, L"ERROR CreateWindowEx", L"ERROR", MB_ICONWARNING);
		return FALSE;
	}

	textChanged = false;

	return TRUE;
}

void COverlappedWindow::Show(int cmdShow)
{
	::ShowWindow(handle, cmdShow);
	::UpdateWindow(handle);

	MSG msg = {};
	BOOL getMessageResult = 0;

	while((getMessageResult = ::GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (!::TranslateAccelerator(handle, haccel, &msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

LRESULT __stdcall COverlappedWindow::MsgRouter(HWND handle, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	COverlappedWindow *thisWindow = NULL;

	if(uMsg == WM_GETMINMAXINFO)
	{
		return ::DefWindowProc(handle, uMsg, wParam, lParam);
	}

	if(uMsg == WM_NCCREATE)
	{
		LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		void *lpCreateParam = createStruct->lpCreateParams;
		thisWindow = reinterpret_cast<COverlappedWindow *>(lpCreateParam);

		::SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisWindow));

		thisWindow->storeHandle(handle);
	}
	else
	{
		thisWindow = reinterpret_cast<COverlappedWindow *>(::GetWindowLongPtr(handle, GWLP_USERDATA));
	}

	return thisWindow->wndProc(uMsg, wParam, lParam);
}

LRESULT __stdcall COverlappedWindow::wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{		
	switch(uMsg)
	{
	case WM_CREATE:
		onCreate();
		return 0;

	case WM_CLOSE:
		onClose();
		return 0;

	case WM_SIZE:
		onSize();
		return 0;

	case WM_DESTROY:
		onDestroy();
		return 0;

	case WM_PAINT:
		handlePaint();
		return 0;

	case WM_COMMAND:
		onCommand(wParam, lParam);
		return 0;

	default:
		return ::DefWindowProc(handle, uMsg, wParam, lParam);
	}
}

void COverlappedWindow::storeHandle(HWND _handle)
{
	handle = _handle;
}

void COverlappedWindow::onCreate()
{
	handleMenu = ::LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));

	if (!::SetMenu(handle, handleMenu))
	{
		::MessageBox(NULL, L"ERROR LoadMenu", L"ERROR", MB_ICONERROR);
		return;
	}

	haccel = ::LoadAccelerators(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_ACCELERATOR1));

	if (haccel == NULL)
	{
		::MessageBox(NULL, L"ERROR LoadAccelerators", L"ERROR", MB_ICONERROR);
		return;
	}

	RECT rect;
	::GetClientRect(handle, &rect);

	handleEdit = ::CreateWindowEx(0, L"edit", NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_MULTILINE,
		0, 0, rect.right - rect.left, rect.bottom - rect.top,
		handle, NULL, GetModuleHandle(NULL), NULL);

	if(handleEdit == NULL)
	{
		::MessageBox(NULL, L"ERROR CreateWindowEx", L"ERROR", MB_ICONWARNING);
		return;
	}

	HRSRC myResource = ::FindResource(NULL, MAKEINTRESOURCE(IDR_CUSTOM1), L"CUSTOM");
	HGLOBAL myResourceData = ::LoadResource(NULL, myResource);
	wchar_t* sampleText = (wchar_t*) ::LockResource(myResourceData);
	UINT lenText = ::SizeofResource(GetModuleHandle(NULL), myResource);

	::SetWindowText(handleEdit, (LPCWSTR)sampleText);
}

void COverlappedWindow::onClose()
{
	if(textChanged)
	{
		UINT buttonPressed = ::MessageBox(handle, L"Do you want to save your work?", L"My application", MB_ICONQUESTION | MB_YESNOCANCEL);

		if(buttonPressed == IDYES)
		{
			saveText();
			::DestroyWindow(handle);
		}
		if(buttonPressed == IDNO)
		{
			::DestroyWindow(handle);
		}
		if (buttonPressed == IDCANCEL)
		{
			return;
		}
	}
	else
	{
		::DestroyWindow(handle);
	}
}

void COverlappedWindow::saveText()
{
	DWORD textLength = ::SendMessage(handleEdit, WM_GETTEXTLENGTH, 0, 0);
	textBuffer = new wchar_t[(textLength + 1)*sizeof(wchar_t)];
	::SendMessageW(handleEdit, WM_GETTEXT, (WPARAM)textLength + 1, (LPARAM) textBuffer);
	
	OPENFILENAME Ofn;
	wchar_t fileName[MAX_PATH] = L"";
	memset(&Ofn, 0, sizeof(OPENFILENAME));

	Ofn.lStructSize = sizeof(OPENFILENAME);
	Ofn.hwndOwner = handleEdit;
	Ofn.lpstrFilter = (LPCWSTR)L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	Ofn.lpstrFile = fileName;
	Ofn.nMaxFile = MAX_PATH;
	Ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	Ofn.lpstrDefExt = (LPCWSTR)L"txt";

	if (!GetSaveFileName(&Ofn))
	{
		::MessageBox(NULL, L"ERROR GetSaveFileName", L"ERROR", MB_ICONWARNING);

		return;
	}
	
	HANDLE fileHandle = ::CreateFileW((LPCWSTR) fileName, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		::MessageBox(NULL, L"ERROR CreateFile", L"ERROR", MB_ICONWARNING);

		return;
	}

	DWORD nOfBytesWritten = 0;

	int BOM = 0xfeff;

	if (::WriteFile(fileHandle, &BOM, sizeof(short), &nOfBytesWritten, NULL) == false || // set encoding
		::WriteFile(fileHandle, textBuffer, textLength*sizeof(wchar_t) + 1, &nOfBytesWritten, NULL) == false)
	{
		::MessageBox(NULL, L"ERROR WriteFile", L"ERROR", MB_ICONWARNING);

		return;
	}
	
	textChanged = false;

	::CloseHandle(fileHandle);
	delete[] textBuffer;
}

void COverlappedWindow::onCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case ID_FILE_EXIT:
		onClose();
		break;

	case ID_FILE_SAVE:
		saveText();
		::ModifyMenu(::GetMenu(handle), 0, MF_BYPOSITION | MF_GRAYED, NULL, MF_STRING);
		::EnableMenuItem(::GetMenu(handle), ID_FILE_EXIT, MF_DISABLED | MF_GRAYED);
		break;

	case IDCLOSE:
		onDestroy();
		break;
	}

	if(EN_CHANGE == HIWORD(wParam))
	{
		textChanged = true;
	}

	if(HIWORD(wParam) == EN_ERRSPACE || HIWORD(wParam) == EN_MAXTEXT)
	{
		::MessageBox(handleEdit, L"Edit control out of space", L"ERROR", MB_OK | MB_ICONSTOP);
	}
}

void COverlappedWindow::onSize()
{
	RECT rect;
	int width = 0, height = 0;

	if(GetWindowRect(handle, &rect))
	{
		width = rect.right - rect.left;

		height = rect.bottom - rect.top;
	}

	MoveWindow(handleEdit, 0, 0, width, height, TRUE);
}

void COverlappedWindow::handlePaint()
{
	PAINTSTRUCT paintStruct;

	HDC hdc = ::BeginPaint(handle, &paintStruct);

	RECT rect;
	::GetClientRect(handle, &rect);

	int win_width = rect.right - rect.left;
	int win_height = rect.bottom + rect.left;
	
	HDC Memhdc = ::CreateCompatibleDC(hdc);

	HBITMAP Membitmap = ::CreateCompatibleBitmap(hdc, win_width, win_height);

	::SelectObject(Memhdc, Membitmap);

	::FillRect(Memhdc, &paintStruct.rcPaint,(HBRUSH)COLOR_WINDOW);

	::BitBlt(hdc, 0, 0, win_width, win_height, Memhdc, 0, 0, SRCCOPY);

	::DeleteObject(Membitmap);
	::DeleteDC(Memhdc);
	
	::EndPaint(handle, &paintStruct);
}

void COverlappedWindow::onDestroy()
{
	::PostQuitMessage(0);
}