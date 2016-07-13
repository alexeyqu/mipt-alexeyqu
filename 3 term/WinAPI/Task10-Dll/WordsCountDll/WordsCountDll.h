#pragma once

#include <windows.h>
#define WORDSCOUNT_EXPORTS
#ifdef WORDSCOUNT_EXPORTS
#define WORDSCOUNT_API __declspec(dllexport)
#else
#define WORDSCOUNT_API __declspec(dllimport)
#endif

extern "C" WORDSCOUNT_API int wordsCount(const wchar_t *text);

bool blank(wchar_t symbol);