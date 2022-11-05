#include "winwrap_encoding.h"

#include "dprintf.hpp"

bool WinEncoding::convertMultiByteToWCHAR(const char* in_mb, std::shared_ptr<WCHAR>* out_wchar) {
	bool result = false;
	UINT codepage_candidate[] = { CP_ACP, CP_UTF8, CP_THREAD_ACP, CP_OEMCP };

	if (in_mb == nullptr)
		return false;

	for (int codepage : codepage_candidate) {
		if (convertMultiByteToWCHAR(in_mb, codepage, out_wchar))
			return true;
	}
	return false;
}

bool WinEncoding::convertMultiByteToWCHAR(const char* in_mb, UINT codepage, std::shared_ptr<WCHAR>* out_wchar) {
	bool result = false;
	DWORD getlasterror = ERROR_UNHANDLED_ERROR;
	int out_wchar_cnt = 0;
	std::shared_ptr<WCHAR> buf(nullptr);

	do {
		if (in_mb == nullptr) {
			break;
		}
		if ((out_wchar_cnt = MultiByteToWideChar(codepage, MB_ERR_INVALID_CHARS, in_mb, -1, NULL, 0)) <= 0) {
			dprintf("[WinEncoding::convertMultiByteToWCHAR] MultiByteToWideChar get length failed : 0x%08x", getlasterror);
			break;
		}
		buf = std::shared_ptr<WCHAR>(new WCHAR[out_wchar_cnt], std::default_delete<WCHAR[]>());
		if (MultiByteToWideChar(codepage, MB_ERR_INVALID_CHARS, in_mb, -1, buf.get(), out_wchar_cnt) == 0) {
			dprintf("[WinEncoding::convertMultiByteToWCHAR] MultiByteToWideChar conversion failed : 0x%08x", getlasterror);
			break;
		}
		*out_wchar = buf;
		result = true;
	} while (0);
	
	return result;
}

bool WinEncoding::convertWCHARtoMultiByte(const WCHAR* in_wchar, std::shared_ptr<char>* out_mb)
{
	bool result = false;
	UINT codepage_candidate[] = { CP_ACP, CP_UTF8, CP_THREAD_ACP, CP_OEMCP };

	if (in_wchar == nullptr)
		return false;

	for (int codepage : codepage_candidate){
		if (convertWCHARtoMultiByte(in_wchar, codepage, out_mb))
			return true;
	}
	return false;
}

bool WinEncoding::convertWCHARtoMultiByte(const WCHAR* in_wchar, UINT codepage, std::shared_ptr<char>* out_mb) {
	bool result = false;
	DWORD getlasterror = ERROR_UNHANDLED_ERROR;
	int out_mb_cnt = 0;
	std::shared_ptr<char> buf(nullptr);

	do {
		if (in_wchar == nullptr) {
			break;
		}
		if ((out_mb_cnt = WideCharToMultiByte(codepage, WC_ERR_INVALID_CHARS, in_wchar, -1, NULL, 0, NULL, NULL)) <= 0) {
			dprintf("[WinEncoding::convertWCHARtoMultiByte] WideCharToMultiByte get length failed : 0x%08x", getlasterror);
			break;
		}
		buf = std::shared_ptr<char>(new char[out_mb_cnt], std::default_delete<char[]>());
		if (WideCharToMultiByte(codepage, WC_ERR_INVALID_CHARS, in_wchar, -1, buf.get(), out_mb_cnt, NULL, FALSE) == 0) {
			break;
		}
		*out_mb = buf;
		result = true;
	} while (0);
	
	return result;
}

WinEncoding::~WinEncoding()
{
}