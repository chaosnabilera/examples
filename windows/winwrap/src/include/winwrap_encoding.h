#pragma once
#ifndef __WINWRAP_ENCODING_H__
#define __WINWRAP_ENCODING_H__

#include <Windows.h>

#include <memory>

class WinEncoding {
public:
    static bool convertMultiByteToWCHAR(const char* in_mb, std::shared_ptr<WCHAR>* out_wchar);
    static bool convertMultiByteToWCHAR(const char* in_mb, UINT codepage, std::shared_ptr<WCHAR>* out_wchar);
    static bool convertWCHARtoMultiByte(const WCHAR* in_wchar, std::shared_ptr<char>* out_mb);
    static bool convertWCHARtoMultiByte(const WCHAR* in_wchar, UINT codepage, std::shared_ptr<char>* out_mb);
    ~WinEncoding();
private:
    WinEncoding();
};

#endif