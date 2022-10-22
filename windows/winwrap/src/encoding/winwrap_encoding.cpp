#include "winwrap_encoding.h"

#include "dprintf.hpp"

WCHAR* WinEncoding::utf8_to_wchar(const char* in)
{
	WCHAR* out = nullptr;
	UINT codepage[] = { CP_ACP, CP_OEMCP, CP_THREAD_ACP, CP_UTF8 };
	DWORD getlasterror = ERROR_UNHANDLED_ERROR;
	int len = 0;

	if (in == nullptr)
		return nullptr;

	for (int cp : codepage) {
		if ((len = MultiByteToWideChar(cp, MB_ERR_INVALID_CHARS, in, -1, NULL, 0)) <= 0) {
			if ((getlasterror = GetLastError()) == ERROR_NO_UNICODE_TRANSLATION)
				continue;
				
			dprintf("[WinEncoding::utf8_to_wchar] MultiByteToWideChar failed : 0x%08x", getlasterror);
			break;
		}

		out = new WCHAR[len];
			
		if (MultiByteToWideChar(cp, 0, in, -1, out, len) == 0) {
			delete[] out;
			out = nullptr;
			break;
		}
			
		// at this stage, all is well
		break;
	}

	return out;
}

char* WinEncoding::wchar_to_utf8(const WCHAR* in)
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