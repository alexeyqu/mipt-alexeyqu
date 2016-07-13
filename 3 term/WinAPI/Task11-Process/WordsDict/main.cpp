#include "WordsDict.h"

int main()
{
	std::wstring textReadyEventName = L"Global\\TextReady" + std::to_wstring(::GetCurrentProcessId());

	HANDLE textReadyEvent = ::CreateEvent(NULL, FALSE, FALSE, (LPWSTR)textReadyEventName.c_str());

	std::wstring textClearedEventName = L"Global\\TextCleared" + std::to_wstring(::GetCurrentProcessId());

	HANDLE textClearedEvent = ::CreateEvent(NULL, FALSE, FALSE, (LPWSTR)textClearedEventName.c_str());

	std::wstring terminateEventName = L"Global\\Terminate";

	HANDLE terminateEvent = ::CreateEvent(NULL, TRUE, FALSE, (LPWSTR)terminateEventName.c_str());

	HANDLE events[2];
	events[0] = textReadyEvent;
	events[1] = terminateEvent;

	while(1)
	{
		std::cout << "IN CYCLE: " << GetCurrentProcessId() << std::endl;

		DWORD waitRes = ::WaitForMultipleObjects(2, events, FALSE, INFINITE);

		std::cout << "WAIT RES ERROR " << GetCurrentProcessId() << " " << GetLastError() << std::endl;
		std::cout << "WAIT RES " << GetCurrentProcessId() << " " << waitRes << std::endl;
		std::cout << "AFTER WAIT: " << GetCurrentProcessId() << std::endl;

	//	std::cin.get();

		switch (waitRes) 
		{
			case WAIT_OBJECT_0:
				clearForeignWords();
				::SetEvent(textClearedEvent);
				break;
	
			case WAIT_OBJECT_0 + 1:
				exit(0);
		}
	}

	std::cin.get();

	return 0;
}