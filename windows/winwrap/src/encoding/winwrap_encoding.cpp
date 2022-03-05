#include "winwrap_encoding.h"

WCHAR* utf8_to_wchar(const char* in)
{
	WCHAR* out = nullptr;
	int len = 0;

	do {
		if (in == nullptr)
			break;
		if ((len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, in, -1, NULL, 0)) <= 0)
			break;
		if ((out = (WCHAR*)malloc(sizeof(WCHAR) * len)) == nullptr)
			break;
		if (MultiByteToWideChar(CP_UTF8, 0, in, -1, out, len) == 0) {
			free(out); out = nullptr;
			break;
		}
	} while (0);

	return out;
}

char* wchar_to_utf8(const WCHAR* in)
{
	char* out = nullptr;
	int len = 0;

	do {
		if (in == nullptr)
			break;
		if ((len = WideCharToMultiByte(CP_UTF8, 0, in, -1, NULL, 0, NULL, NULL)) <= 0)
			break;
		if ((out = (char*)malloc(len)) == nullptr)
			break;
		if (WideCharToMultiByte(CP_UTF8, 0, in, -1, out, len, NULL, FALSE) == 0) {
			free(out); out = nullptr;
			break;
		}
	} while (0);

	return out;
}