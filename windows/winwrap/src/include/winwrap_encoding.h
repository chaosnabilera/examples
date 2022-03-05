#pragma once

#include <Windows.h>

class WinEncoding {
public:
	static WCHAR* utf8_to_wchar(const char* in);
	static char* wchar_to_utf8(const WCHAR* in);
private:
	WinEncoding();
	~WinEncoding();
};