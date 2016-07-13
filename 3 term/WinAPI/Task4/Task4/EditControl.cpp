#include "EditControl.h"

bool COverlappedWindow::RegisterClass(HINSTANCE hInstance, const wchar_t className[])
{
	WNDCLASSEX wndClass = {};

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = MsgRouter;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = ::LoadCursor(NULL,(LPTSTR)IDC_ARROW);
	wndClass.hbrBackground =(HBRUSH) ::GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = className;
	wndClass.hIconSm = ::LoadIcon(NULL, IDI_APPLICATION);

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
	handle = ::CreateWindowEx(
		0,			// Optional window styles	
		className,	// Window class
		L"Task 1 example window",	 // Window text
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
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
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
		OnDestroy();
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
	RECT rect;
	::GetClientRect(handle, &rect);

	handleEdit = ::CreateWindowEx(0, L"edit", NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_MULTILINE,
		0, 0, rect.right - rect.left, rect.bottom - rect.top,
		handle, NULL, GetModuleHandle(NULL), NULL);

	if(handleEdit == NULL)
	{
		::MessageBox(NULL, L"ERROR CreateWindowEx", L"ERROR", MB_ICONWARNING);
	}

	::SendMessage(handleEdit, WM_SETTEXT, 0,(LPARAM)L"HELLO, EDITCONTROL");
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
		wchar_t err[256];
		memset(err, 0, 256);
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL);
		int msgboxID = MessageBoxW(
			NULL,
			err,
			(LPCWSTR)L"HELL",
			MB_OK
			);

		::MessageBox(NULL, L"ERROR GetSaveFileName", L"ERROR", MB_ICONWARNING);
	}
	
	HANDLE fileHandle = ::CreateFileW((LPCWSTR) fileName, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		::MessageBox(NULL, L"ERROR CreateFile", L"ERROR", MB_ICONWARNING);
	}

	DWORD nOfBytesWritten = 0;

	short BOM = 0xfeff;
	::WriteFile(fileHandle, &BOM, sizeof(short), &nOfBytesWritten, 0); // set encoding

	if (::WriteFile(fileHandle, textBuffer, textLength*sizeof(wchar_t) + 1, &nOfBytesWritten, NULL) == false)
	{
		::MessageBox(NULL, L"ERROR WriteFile", L"ERROR", MB_ICONWARNING);
	}
	
	textChanged = false;

	::CloseHandle(fileHandle);
	delete[] textBuffer;
}

void COverlappedWindow::onCommand(WPARAM wParam, LPARAM lParam)
{
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

void COverlappedWindow::OnDestroy()
{
	::PostQuitMessage(0);
}