// WordsCountDll.cpp: определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include "WordsCountDll.h"
#include <cwchar>

int wordsCount(const wchar_t * text)
{
	int textLen = wcslen(text);

	if (textLen == 0)
	{
		return 0;
	}

	int nOfWords = 0;

	for (int i = 0; i < textLen - 1; i++)
	{
		if (!blank(text[i]) && blank(text[i + 1]))
		{
			nOfWords++;
		}
	}

	if (!blank(text[textLen - 1]))
	{
		nOfWords++;
	}

	return nOfWords;
}

bool blank(wchar_t symbol)
{
	if (symbol == (wchar_t) ' ' ||
		symbol == (wchar_t) '\n' ||
		symbol == (wchar_t) '\t')
	{
		return true;
	}

	return false;
}

BOOL __stdcall DLLMain(HINSTANCE hInstance, DWORD reason, LPVOID reserved)
{
	return TRUE;
}