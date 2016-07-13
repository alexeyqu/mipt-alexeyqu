#include "MyNotepad.h"
#include "resource.h"

#define MAPFILEBUFFER 2048

bool CMyNotepad::RegisterClass(HINSTANCE hInstance, const wchar_t className[])
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
	wndClass.hCursor = ::LoadCursor(NULL, (LPTSTR)IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH) ::GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wndClass.lpszClassName = className;

	const ATOM atom = ::RegisterClassEx(&wndClass);

	if (atom == NULL)
	{
		::MessageBox(NULL, L"ERROR RegisterClassEx", L"Error", MB_ICONWARNING);
		return FALSE;
	}

	return TRUE;
}

bool CMyNotepad::Create(HINSTANCE hInstance, const wchar_t className[])
{
	handle = ::CreateWindowEx(
		0,			// Optional window styles	
		className,	// Window class
		L"MyNotepad",	 // Window text
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

	textChanged = false;

	procsInit();

	return TRUE;
}

void CMyNotepad::procsInit()
{
	for (int i = 0; i < N_OF_PROCESSES; i++)
	{
		STARTUPINFO startupInfo;
		PROCESS_INFORMATION procInfo;

		::ZeroMemory(&startupInfo, sizeof(startupInfo));
		::ZeroMemory(&procInfo, sizeof(procInfo));

		startupInfo.cb = sizeof(startupInfo);

		if (!::CreateProcess(L"C:\\workspace\\ABBYY\\Task11-Process\\Debug\\WordsDict.exe",
			NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &procInfo))
		{
			::MessageBox(NULL, L"ERROR CreateProcess", L"ERROR", MB_ICONWARNING);
			return ;
		}

		procsInfo[i] = procInfo;

		std::wstring fileName = L"Local\\Text" + std::to_wstring(procInfo.dwProcessId);

		mappedFileNames[i] = fileName.c_str();

		HANDLE hMapFile = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAPFILEBUFFER, (LPTSTR)fileName.c_str());

		if (hMapFile == NULL)
		{
			::MessageBox(NULL, L"ERROR CreateFileMapping", L"ERROR", MB_ICONWARNING);
			return;
		}

		mappedFiles[i] = hMapFile;
	}
}

void CMyNotepad::Show(int cmdShow)
{
	::ShowWindow(handle, cmdShow);
	::UpdateWindow(handle);

	MSG msg = {};
	BOOL getMessageResult = 0;

	while ((getMessageResult = ::GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (!::TranslateAccelerator(handle, haccel, &msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

LRESULT __stdcall CMyNotepad::MsgRouter(HWND handle, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CMyNotepad *thisWindow = NULL;

	if (uMsg == WM_GETMINMAXINFO)
	{
		return ::DefWindowProc(handle, uMsg, wParam, lParam);
	}

	if (uMsg == WM_NCCREATE)
	{
		LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		void *lpCreateParam = createStruct->lpCreateParams;
		thisWindow = reinterpret_cast<CMyNotepad *>(lpCreateParam);

		::SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisWindow));

		thisWindow->storeHandle(handle);
	}
	else
	{
		thisWindow = reinterpret_cast<CMyNotepad *>(::GetWindowLongPtr(handle, GWLP_USERDATA));
	}

	return thisWindow->wndProc(uMsg, wParam, lParam);
}

LRESULT __stdcall CMyNotepad::wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
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

void CMyNotepad::storeHandle(HWND _handle)
{
	handle = _handle;
}

void CMyNotepad::onCreate()
{
	HMENU myMenu = ::LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));

	if (!::SetMenu(handle, myMenu))
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

	if (handleEdit == NULL)
	{
		::MessageBox(NULL, L"ERROR CreateWindowEx", L"ERROR", MB_ICONWARNING);
		return;
	}
/*
	HRSRC myResource = ::FindResource(NULL, MAKEINTRESOURCE(IDR_CUSTOM1), L"CUSTOM");
	HGLOBAL myResourceData = ::LoadResource(NULL, myResource);
	wchar_t* sampleText = (wchar_t*) ::LockResource(myResourceData);
	UINT lenText = ::SizeofResource(GetModuleHandle(NULL), myResource);

	::SetWindowText(handleEdit, (LPCWSTR)sampleText);
*/
}

void CMyNotepad::onClose()
{
	if (textChanged)
	{
		UINT buttonPressed = ::MessageBox(handle, L"Do you want to save your work?", L"My application", MB_ICONQUESTION | MB_YESNOCANCEL);

		if (buttonPressed == IDYES)
		{
			saveText();
			::DestroyWindow(handle);
		}
		if (buttonPressed == IDNO)
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

void CMyNotepad::saveText()
{
	DWORD textLength = (DWORD) ::SendMessage(handleEdit, WM_GETTEXTLENGTH, 0, 0);
	textBuffer = new wchar_t[(textLength + 1)*sizeof(wchar_t)];
	::SendMessageW(handleEdit, WM_GETTEXT, (WPARAM)textLength + 1, (LPARAM)textBuffer);

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

	if (!::GetSaveFileName(&Ofn))
	{
		::MessageBox(NULL, L"ERROR GetSaveFileName", L"ERROR", MB_ICONWARNING);

		return;
	}

	HANDLE fileHandle = ::CreateFileW((LPCWSTR)fileName, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

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

void CMyNotepad::openFile()
{
	OPENFILENAME Ofn;
	wchar_t fileName[MAX_PATH] = L"";
	memset(&Ofn, 0, sizeof(OPENFILENAME));

	::ZeroMemory(&Ofn, sizeof(Ofn));
	Ofn.lStructSize = sizeof(Ofn);
	Ofn.hwndOwner = handle;
	Ofn.lpstrFile = fileName;
	Ofn.lpstrFile[0] = '\0';
	Ofn.nMaxFile = sizeof(fileName);
	Ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
	Ofn.nFilterIndex = 1;
	Ofn.lpstrFileTitle = NULL;
	Ofn.nMaxFileTitle = 0;
	Ofn.lpstrInitialDir = NULL;
	Ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (!::GetOpenFileName(&Ofn))
	{
		::MessageBox(NULL, L"ERROR GetOpenFileName", L"ERROR", MB_ICONWARNING);

		return;
	}

	HANDLE fileHandle = ::CreateFile((LPWSTR)Ofn.lpstrFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		::MessageBox(NULL, L"ERROR CreateFile", L"ERROR", MB_ICONWARNING);

		return;
	}

	DWORD fileSize = ::GetFileSize(fileHandle, NULL);

	if (fileSize == INVALID_FILE_SIZE)
	{
		::MessageBox(NULL, L"ERROR GetFileSize", L"ERROR", MB_ICONWARNING);

		return;
	}

	DWORD fileReadSize = 0;

	wchar_t* textBufferRead = new wchar_t[(fileSize)*sizeof(wchar_t)];

	memset(textBufferRead, 0, (fileSize)*sizeof(wchar_t));

	if (!::ReadFile(fileHandle, textBufferRead, fileSize, &fileReadSize, NULL))
	{
		::MessageBox(NULL, L"ERROR ReadFileEx", L"ERROR", MB_ICONWARNING);

		return;
	}

//	myOutputDebugPrintf(L"%d %d %d %d", sizeof(textBufferRead), sizeof(wchar_t), fileSize, fileReadSize);

//	::MessageBox(NULL, textBufferRead, L"ERROR", MB_ICONWARNING);

//	wchar_t *text = new wchar_t[fileReadSize * sizeof(wchar_t)];

	::SetWindowText(handleEdit, textBufferRead);

	textChanged = false;

	::CloseHandle(fileHandle);

	delete[] textBufferRead;
}

int CMyNotepad::myOutputDebugPrintf(const wchar_t format[], ...)
{
	if (!format) return 0;

	bool msgbox = (*format == '\a') ? (format++, true) : false;
	bool print = (*format == '\f') ? (format++, true) : false;

	wchar_t str[MAX_TEXT_SIZE] = L"";

	va_list arg; va_start(arg, format);
	int n = _vsnwprintf_s(str, sizeof(str) - 1, format, arg);
	va_end(arg);

	OutputDebugStringW(str);

	if (print)
		fprintf(stderr, "%S", str);

	if (msgbox)
		::MessageBox(handle, str, L"ERROR?", MB_ICONEXCLAMATION | MB_TOPMOST);

	return n;
}

void CMyNotepad::onCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case ID_FILE_EXIT:
		onClose();
		break;

	case ID_FILE_SAVE:
		saveText();
		break;

	case ID_FILE_OPEN:
		openFile();
		break;

	case ID_FILE_CLEAR:
		onClear();
		break;

	case IDCLOSE:
		onDestroy();
		break;
	}

	if (EN_CHANGE == HIWORD(wParam))
	{
		textChanged = true;
	}

	if (HIWORD(wParam) == EN_ERRSPACE || HIWORD(wParam) == EN_MAXTEXT)
	{
		::MessageBox(handleEdit, L"Edit control out of space", L"ERROR", MB_OK | MB_ICONSTOP);
	}
}

void CMyNotepad::onClear()
{
	for (int i = 0; i < N_OF_PROCESSES; i++)
	{
		PROCESS_INFORMATION procInfo = procsInfo[i];

		std::wstring textReadyEventName = L"Global\\TextReady";
		textReadyEventName += std::to_wstring(procInfo.dwProcessId);
		HANDLE eventTextPartReady = ::OpenEvent(EVENT_ALL_ACCESS, false, (LPCWSTR)textReadyEventName.c_str());
		eventsTextReady[i] = eventTextPartReady;

		std::wstring eventTextClearedName = L"Global\\TextCleared";
		eventTextClearedName += std::to_wstring(procInfo.dwProcessId);
		HANDLE eventTextPartCleared = ::OpenEvent(EVENT_ALL_ACCESS, false, (LPCWSTR)eventTextClearedName.c_str());
		eventsTextClear[i] = eventTextPartCleared;
	}

	std::wstring eventTerminateName = L"Global\\Terminate";
	HANDLE eventTerminate = ::OpenEvent(EVENT_ALL_ACCESS, false, (LPCWSTR)eventTerminateName.c_str());

	DWORD len = SendMessage(handleEdit, WM_GETTEXTLENGTH, 0, 0);

	if (len != 0) 
	{
		textBuffer = new wchar_t[len];
		::SendMessage(handleEdit, WM_GETTEXT, (WPARAM)len + 1, (LPARAM) textBuffer);
		std::wstring wText = textBuffer;
		std::string text = std::string(wText.begin(), wText.end());
		std::istringstream iss(text);
		std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
			std::istream_iterator<std::string>{} };

		std::wstring textChunk1;
		std::wstring textChunk2;
		std::wstring textChunk3;
		std::wstring textChunk4;

		int wordsInParts = tokens.size() / N_OF_PROCESSES;
		for (int i = 0; i < tokens.size(); i++) 
		{
			std::wstring tmp = std::wstring(tokens[i].begin(), tokens[i].end());
			if (i <= wordsInParts) 
			{
				textChunk1 += tmp;
				textChunk1 += L" ";
			}
			else if (i > wordsInParts && i <= 2 * wordsInParts) 
			{
				textChunk2 += tmp;
				textChunk2 += L" ";
			}
			else if (i > 2 * wordsInParts && i <= 3 * wordsInParts) 
			{
				textChunk3 += tmp;
				textChunk3 += L" ";
			}
			else if (i > 3 * wordsInParts) 
			{
				textChunk4 += tmp;
				textChunk4 += L" ";
			}
		}

//		::MessageBox(handle, (LPCWSTR) textChunk1.c_str(), L"ERROR?", MB_ICONEXCLAMATION | MB_TOPMOST);

		for (int i = 0; i < N_OF_PROCESSES; i++) 
		{
			LPCTSTR pBuf;
			pBuf = (LPTSTR)MapViewOfFile(mappedFiles[i],
				FILE_MAP_ALL_ACCESS,
				0,
				0,
				MAPFILEBUFFER);

			char a[MAPFILEBUFFER];
			memset(a, 0, MAPFILEBUFFER);
			::CopyMemory((PVOID)pBuf, (PVOID)a, MAPFILEBUFFER);

			switch (i) 
			{
				case 0:
				{
					LPCTSTR _text1 = (LPTSTR)textChunk1.c_str();

			//		::MessageBox(handle, (LPWSTR)_text1, L"", MB_ICONASTERISK);

			//		::MessageBox(handle, (LPWSTR)(lstrlen(_text1) * sizeof(TCHAR)), L"", MB_ICONASTERISK);
					::CopyMemory((PVOID)pBuf, (PVOID)_text1, (lstrlen(_text1) * sizeof(TCHAR)));
					break;
				}
				case 1:
				{
					LPTSTR _text2 = (LPTSTR)textChunk2.c_str();
					::CopyMemory((PVOID)pBuf, (PVOID)_text2, (lstrlen(_text2) * sizeof(TCHAR)));
					break;
				}
				case 2:
				{
					LPTSTR _text3 = (LPTSTR)textChunk3.c_str();
					::CopyMemory((PVOID)pBuf, (PVOID)_text3, (lstrlen(_text3) * sizeof(TCHAR)));
					break;
				}
				case 3:
				{
					LPTSTR _text4 = (LPTSTR)textChunk4.c_str();
					::CopyMemory((PVOID)pBuf, (PVOID)_text4, (lstrlen(_text4) * sizeof(TCHAR)));
					break;
				}
			}
			::SetEvent(eventsTextReady[i]);
			DWORD err = ::GetLastError();
		}

		BOOL ready1 = FALSE;
		BOOL ready2 = FALSE;
		BOOL ready3 = FALSE;
		BOOL ready4 = FALSE;

		LPTSTR part1 = 0;
		LPTSTR part2 = 0;
		LPTSTR part3 = 0;
		LPTSTR part4 = 0;

		do 
		{
			DWORD waitResult = ::WaitForMultipleObjects(N_OF_PROCESSES, eventsTextClear, FALSE, INFINITE);

			switch (waitResult) 
			{
				case WAIT_OBJECT_0:
				{
					part1 = (LPTSTR)MapViewOfFile(mappedFiles[0],   
						FILE_MAP_ALL_ACCESS,
						0,
						0,
						MAPFILEBUFFER);
					ready1 = TRUE;
					break;
				}
				case WAIT_OBJECT_0 + 1:
				{
					part2 = (LPTSTR)MapViewOfFile(mappedFiles[1],   
						FILE_MAP_ALL_ACCESS,
						0,
						0,
						MAPFILEBUFFER);
					ready2 = TRUE;
					break;
				}
				case WAIT_OBJECT_0 + 2:
				{
					part3 = (LPTSTR)MapViewOfFile(mappedFiles[2], 
						FILE_MAP_ALL_ACCESS,
						0,
						0,
						MAPFILEBUFFER);
					ready3 = TRUE;
					break;
				}
				case WAIT_OBJECT_0 + 3:
				{
					part4 = (LPTSTR)MapViewOfFile(mappedFiles[3], 
						FILE_MAP_ALL_ACCESS, 
						0,
						0,
						MAPFILEBUFFER);
					ready4 = TRUE;
					break;
				}
			}
		} while (!(ready1 && ready2 && ready3 && ready4));


//		::MessageBox(handle, L"Gone", L"ERROR?", MB_ICONEXCLAMATION | MB_TOPMOST);

		std::wstring w1 = part1;
		std::wstring w2 = part2;
		std::wstring w3 = part3;
		std::wstring w4 = part4;



//		::MessageBox(handle, (LPCWSTR)w1.c_str(), L"ERROR?", MB_ICONEXCLAMATION | MB_TOPMOST);


		std::wstring result = w1;
		if (w1.size() != 0) 
		{
			result += L" ";
		}

		result += w2;
		if (w2.size() != 0) 
		{
			result += L" ";
		}

		result += w3;
		if (w3.size() != 0) 
		{
			result += L" ";
		}

		result += w4;

		::SetWindowText(handleEdit, (LPCWSTR)result.c_str());
	}
}

void CMyNotepad::onSize()
{
	RECT rect;
	int width = 0, height = 0;

	if (GetWindowRect(handle, &rect))
	{
		width = rect.right - rect.left;

		height = rect.bottom - rect.top;
	}

	MoveWindow(handleEdit, 0, 0, width, height, TRUE);
}

void CMyNotepad::handlePaint()
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

	::BitBlt(hdc, 0, 0, win_width, win_height, Memhdc, 0, 0, SRCCOPY);

	::DeleteObject(Membitmap);
	::DeleteDC(Memhdc);

	::EndPaint(handle, &paintStruct);
}

void CMyNotepad::onDestroy()
{
	::DestroyAcceleratorTable(haccel);

	::PostQuitMessage(0);
}