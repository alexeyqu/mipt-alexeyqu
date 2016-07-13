#include "Ellipse.h"

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

	if (handle == NULL)
	{
		::MessageBox(NULL, L"ERROR CreateWindowEx", L"ERROR", MB_ICONWARNING);
		return FALSE;
	}

	myEllipse.setEllipse(100, 100, 200, 100, 10, 10);

	return TRUE;
}

void COverlappedWindow::Show(int cmdShow)
{
	ShowWindow(handle, cmdShow);

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

	case WM_TIMER:
		onTimer();
		return 0;

	case WM_CLOSE:
		onClose();
		return 0;

	case WM_DESTROY:
		OnDestroy();
		return 0;

	case WM_PAINT:
		handlePaint();
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
	SetTimer(handle, 1, 10, NULL);
}

void COverlappedWindow::onTimer()
{
	RECT rect;
	if (GetClientRect(handle, &rect))
	{
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		if (myEllipse.xCoord < 0)
		{
			myEllipse.xCoord = 0;
			myEllipse.vx *= -1;
		}

		if (myEllipse.xCoord + myEllipse.figWidth > width)
		{
			myEllipse.xCoord = width - myEllipse.figWidth;
			myEllipse.vx *= -1;
		}

		if (myEllipse.yCoord < 0)
		{
			myEllipse.yCoord = 0;
			myEllipse.vy *= -1;
		}

		if (myEllipse.yCoord + myEllipse.figHeight > height)
		{
			myEllipse.yCoord = height - myEllipse.figHeight;
			myEllipse.vy *= -1;
		}
	}

	myEllipse.yCoord += myEllipse.vy;
	myEllipse.xCoord += myEllipse.vx;

	InvalidateRect(handle, NULL, FALSE);
}

void COverlappedWindow::onClose()
{
	if (::MessageBox(handle, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
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

	::FillRect(Memhdc, &paintStruct.rcPaint, (HBRUSH)COLOR_WINDOW);

	HBRUSH hBrush = ::CreateHatchBrush(HS_DIAGCROSS, 0x66FF66);

	::SelectObject(Memhdc, hBrush);

	::Rectangle(Memhdc, myEllipse.xCoord, myEllipse.yCoord, myEllipse.xCoord + myEllipse.figWidth, myEllipse.yCoord + myEllipse.figHeight);
	::Ellipse(Memhdc, myEllipse.xCoord, myEllipse.yCoord, myEllipse.xCoord + myEllipse.figWidth, myEllipse.yCoord + myEllipse.figHeight);

	::BitBlt(hdc, 0, 0, win_width, win_height, Memhdc, 0, 0, SRCCOPY);

	::DeleteObject(hBrush);
	::DeleteObject(Membitmap);
	::DeleteDC(Memhdc);
	
	/*
	::FillRect(hdc, &paintStruct.rcPaint, (HBRUSH)COLOR_WINDOW);

	HBRUSH hBrush = ::CreateHatchBrush(HS_DIAGCROSS, 0x66FF66);

	::SelectObject(hdc, hBrush);

	::Rectangle(hdc, myEllipse.xCoord, myEllipse.yCoord, myEllipse.xCoord + myEllipse.figWidth, myEllipse.yCoord + myEllipse.figHeight);
	::Ellipse(hdc, myEllipse.xCoord, myEllipse.yCoord, myEllipse.xCoord + myEllipse.figWidth, myEllipse.yCoord + myEllipse.figHeight);

	::BitBlt(hdc, 0, 0, win_width, win_height, hdc, 0, 0, SRCCOPY);

	::DeleteObject(hBrush);

	*/
	
	//SelectObject(hdc, GetStockObject (BLACK_BRUSH));

	// it works even after  deleting without re-selecting 0_o

	//Win logo :)
	//SetPixel(hdc, 0, 0, RGB(255, 0, 0));
	//SetPixel(hdc, 1, 0, RGB(0, 255, 0));
	//SetPixel(hdc, 0, 1, RGB(0, 0, 255));
	//SetPixel(hdc, 1, 1, RGB(255, 255, 0));

	::EndPaint(handle, &paintStruct);
}

void COverlappedWindow::OnDestroy()
{
	::PostQuitMessage(0);
}