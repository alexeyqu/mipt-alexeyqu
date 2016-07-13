#include "WordsDict.h"

wchar_t* buffer;
std::set<std::wstring> dict;

void fillDict()
{
	dict = * new std::set<std::wstring>;

	dict.insert(L"aq");
	dict.insert(L"aqaq");
	dict.insert(L"aqaqaq");
}

void printDict()
{
	std::set<std::wstring>::iterator it;

	for (it = dict.begin(); it != dict.end(); ++it)
	{
		std::cout << std::string(it->begin(), it->end()) << std::endl;
	}
}


void clearForeignWords()
{
	fillDict();
	LPCTSTR buf;
	std::wstring fileDictName = L"Local\\Text";
	fileDictName += std::to_wstring(::GetCurrentProcessId());

	HANDLE hMapFileHandle = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, (LPCWSTR)fileDictName.c_str());

	std::cout << "Got it" << std::endl;

	if (hMapFileHandle == NULL)
	{
		std::cout << "ERROR OpenFileMapping" << std::endl;
		exit(1);
	}

	buf = (LPCTSTR) ::MapViewOfFile(hMapFileHandle, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);

	if (buf == NULL)
	{
		std::cout << "ERROR MapViewOfFile" << std::endl;
		exit(1);
	}

	std::wstring wtext = buf;

	std::cout << "\"" << wtext.c_str() << "\"" << std::endl;

	std::string text = std::string(wtext.begin(), wtext.end());

	std::cout << "\"" << text << "\"" << std::endl;

	std::istringstream iss(text);

	std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{} };

	std::wstring resultText;

	printDict();

	for (int i = 0; i < tokens.size(); i++)
	{
		std::wstring tmp = std::wstring(tokens[i].begin(), tokens[i].end());

		std::cout << "\"" << std::string(tmp.begin(), tmp.end()) << "\"" << std::endl;

		if (dict.find(tmp) != dict.end())
		{
			std::cout << "Added" << std::endl;
			resultText += tmp;
			resultText += L" ";
		}
	}

	if (resultText.size() != 0) {
		resultText[resultText.size() - 1] = 0;
	}

	std::cout << "\"" << std::string(resultText.begin(), resultText.end()) << "\"" << std::endl;

	LPTSTR result = (LPTSTR)resultText.c_str();

	char a[BUF_SIZE];
	memset(a, 0, BUF_SIZE);
	::CopyMemory((PVOID)buf, (PVOID)a, BUF_SIZE);
	::CopyMemory((PVOID)buf, (PVOID)result, (lstrlen(result) * sizeof(TCHAR)));
}