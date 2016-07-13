#include "ParentWindow.h"

bool COverlappedWindow::RegisterClass(HINSTANCE hInstance, const wchar_t className[])
{
	WNDCLASSEX wndClass = {};

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_DBLCLKS;
	wndClass.lpfnWndProc = MsgRouter;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, (LPTSTR)IDC_IBEAM);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = className;
	wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	const ATOM atom = ::RegisterClassEx(&wndClass);

	if (atom == NULL)
	{
		::MessageBox(NULL, L"ERROR RegisterClassEx Parent", L"Error", MB_ICONWARNING);
		return FALSE;
	}

	return TRUE;
}

bool COverlappedWindow::Create(HINSTANCE hInstance, const wchar_t className[])
{
	handle = ::CreateWindowEx(
		0,			// Optional window styles	
		className,	// Window class
		L"Task 3 example parent window",	 // Window text
		WS_OVERLAPPEDWINDOW,	// Window style
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,	// Size and position
		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		this        // Additional application data
		);

	if (handle == NULL)
	{
		::MessageBox(NULL, L"ERROR CreateWindowEx Parent", L"ERROR", MB_ICONWARNING);
		return FALSE;
	}

	return TRUE;
}

void COverlappedWindow::Show(int cmdShow)
{
	ShowWindow(handle, cmdShow);
	UpdateWindow(handle);

	MSG msg = {};
	BOOL getMessageResult = 0;

	while ((getMessageResult = ::GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

LRESULT __stdcall COverlappedWindow::MsgRouter(HWND handle, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	COverlappedWindow *thisWindow = NULL;

	if (uMsg == WM_GETMINMAXINFO)
	{
		return ::DefWindowProc(handle, uMsg, wParam, lParam);
	}

	if (uMsg == WM_NCCREATE)
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
	switch (uMsg)
	{
	case WM_CREATE:
		onCreate();
		return 0;

	case WM_SIZE:
		onSize();
		return 0;

	case WM_CLOSE:
		onClose();
		return 0;

	case WM_DESTROY:
		OnDestroy();
		return 0;

	case WM_PAINT:
		handlePaint();
		ShowWindow(topLeftChild.getHandle(), SW_SHOW);
		ShowWindow(topRightChild.getHandle(), SW_SHOW);
		ShowWindow(bottomLeftChild.getHandle(), SW_SHOW);
		ShowWindow(bottomRightChild.getHandle(), SW_SHOW);
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
	int width = 0, height = 0;
	const wchar_t classChildName[] = L"myPrettyChildClass";

	if (GetWindowRect(handle, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	topLeftChild.RegisterClassW(GetModuleHandle(NULL), classChildName);

	topLeftChild.Create(GetModuleHandle(NULL), classChildName, handle, 0, 0, width/2, height/2);
	topRightChild.Create(GetModuleHandle(NULL), classChildName, handle, width/2, 0, width / 2, height / 2);
	bottomLeftChild.Create(GetModuleHandle(NULL), classChildName, handle, 0, height/2, width / 2, height / 2);
	bottomRightChild.Create(GetModuleHandle(NULL), classChildName, handle, width / 2, height/2, width / 2, height / 2);
}

void COverlappedWindow::onSize()
{
	RECT rect;
	int width = 0, height = 0;

	if (GetWindowRect(handle, &rect))
	{
		width = rect.right - rect.left;
	
		height = rect.bottom - rect.top;
	}
	/*
	SetWindowPos(topLeftChild.getHandle(), HWND_BOTTOM, 0, 0, width / 2, height / 2, SWP_SHOWWINDOW | SWP_FRAMECHANGED); // TODO ARRAY
	SetWindowPos(topRightChild.getHandle(), HWND_BOTTOM, width / 2, 0, width / 2, height / 2, SWP_SHOWWINDOW);
	SetWindowPos(bottomLeftChild.getHandle(), HWND_BOTTOM, 0, height / 2, width / 2, height / 2, SWP_SHOWWINDOW);
	SetWindowPos(bottomRightChild.getHandle(), HWND_BOTTOM, width / 2, height / 2, width / 2, height / 2, SWP_SHOWWINDOW);

	InvalidateRect(topLeftChild.getHandle(), NULL, FALSE);
	InvalidateRect(topRightChild.getHandle(), NULL, FALSE);
	InvalidateRect(bottomLeftChild.getHandle(), NULL, FALSE);
	InvalidateRect(bottomRightChild.getHandle(), NULL, FALSE);*/

	MoveWindow(topLeftChild.getHandle(), 0, 0, width / 2, height / 2, TRUE);
	MoveWindow(topRightChild.getHandle(), width / 2, 0, width / 2, height / 2, TRUE);
	MoveWindow(bottomLeftChild.getHandle(), 0, height / 2, width / 2, height / 2, TRUE);
	MoveWindow(bottomRightChild.getHandle(), width / 2, height / 2, width / 2, height / 2, TRUE);
}

void COverlappedWindow::onClose()
{
	if (::MessageBox(handle, L"Really quit? Parent", L"My application", MB_OKCANCEL) == IDOK)
	{
		::DestroyWindow(handle);
	}
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

	HBRUSH hBrush = ::CreateHatchBrush(HS_HORIZONTAL, 0x000000);

	::FillRect(Memhdc, &paintStruct.rcPaint, (HBRUSH) hBrush);

	::BitBlt(hdc, 0, 0, win_width, win_height, Memhdc, 0, 0, SRCCOPY);

	::DeleteObject(hBrush);
	::DeleteObject(Membitmap);
	::DeleteDC(Memhdc);

	::EndPaint(handle, &paintStruct);
}

void COverlappedWindow::OnDestroy()
{
	::PostQuitMessage(0);
}