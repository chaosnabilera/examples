#include <windows.h>

#include <cstring>
#include <memory>

#include "dprintf.hpp"
#include "winwrap_filesystem.h"

bool WinPath::isFileA(std::string& path) {
	std::wstring wpath(path.begin(), path.end());
	return isFileW(wpath);
}

bool WinPath::isFileW(std::wstring& path) {
	DWORD fattr = GetFileAttributesW(path.c_str());
	return (fattr != INVALID_FILE_ATTRIBUTES) && !(fattr & FILE_ATTRIBUTE_DIRECTORY);
}

bool WinPath::isDirA(std::string& path) {
	std::wstring wpath(path.begin(), path.end());
	return isDirW(wpath);
}

bool WinPath::isDirW(std::wstring& path) {
	DWORD fattr = GetFileAttributesW(path.c_str());
	return (fattr != INVALID_FILE_ATTRIBUTES) && (fattr & FILE_ATTRIBUTE_DIRECTORY);
}

bool WinPath::getFileTimeA(std::string& path, FILETIME* creation_time, FILETIME* last_access_time, FILETIME* last_write_time) {
	std::wstring wpath(path.begin(), path.end());
	bool result = false;
	if (!(result = getFileTimeW(wpath, creation_time, last_access_time, last_write_time))) {
		dprintf("[WinPath::getFileTimeA] Failed");
	}
	return result;
}

bool WinPath::getFileTimeW(std::wstring& path, FILETIME* creation_time, FILETIME* last_access_time, FILETIME* last_write_time) {
	bool result = false;
	HANDLE hfile = INVALID_HANDLE_VALUE;
	DWORD flags_and_attributes = FILE_ATTRIBUTE_READONLY;

	do {
		if (isDirW(path)) { // gets ERROR_ACCESS_DENIED(0x5) if this flag is not set
			flags_and_attributes |= FILE_FLAG_BACKUP_SEMANTICS;
		}

		if ((hfile = CreateFileW(
			path.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			flags_and_attributes,
			NULL
		)) == INVALID_HANDLE_VALUE) {
			dprintf("[WinPath::getFileTimeW] CreateFileW (%S) failed: 0x%08x", path.c_str(), GetLastError());
			break;
		}

		if (!GetFileTime(hfile, creation_time, last_access_time, last_write_time)) {
			dprintf("[WinPath::getFileTimeW] GetFileTime failed : 0x%08x", GetLastError());
			break;
		}

		result = true;
	} while (0);
	
	if (hfile != INVALID_HANDLE_VALUE)
		CloseHandle(hfile);
	
	return result;
}

bool WinPath::getFileTimeAsSystemTimeA(std::string& path, SYSTEMTIME* creation_time, SYSTEMTIME* last_access_time, SYSTEMTIME* last_write_time) {
	std::wstring wpath(path.begin(), path.end());
	bool result = false;
	if (!(result = getFileTimeAsSystemTimeW(wpath, creation_time, last_access_time, last_write_time))) {
		dprintf("[WinPath::getFileTimeAsSystemTimeA] Failed");
	}
	return result;
}

bool WinPath::getFileTimeAsSystemTimeW(std::wstring& path, SYSTEMTIME* creation_time, SYSTEMTIME* last_access_time, SYSTEMTIME* last_write_time) {
	bool result = false;
	FILETIME f_ct = { 0 };
	FILETIME f_at = { 0 };
	FILETIME f_wt = { 0 };

	do {
		if (!getFileTimeW(path, &f_ct, &f_at, &f_wt)) {
			dprintf("[WinPath::getFileTimeAsSystemTimeW] WinPath::getFileTimeW failed");
			break;
		}
		if (creation_time) {
			if (!FileTimeToSystemTime(&f_ct, creation_time)) {
				dprintf("[WinPath::getFileTimeAsSystemTimeW] FileTimeToSystemTime creation_time failed");
				break;
			}
		}
		if (last_access_time) {
			if (!FileTimeToSystemTime(&f_at, last_access_time)) {
				dprintf("[WinPath::getFileTimeAsSystemTimeW] FileTimeToSystemTime last_access_time failed");
				break;
			}
		}
		if (last_write_time) {
			if (!FileTimeToSystemTime(&f_wt, last_write_time)) {
				dprintf("[WinPath::getFileTimeAsSystemTimeW] FileTimeToSystemTime last_write_time failed");
				break;
			}
		}
		result = true;
	} while (0);

	return result;
}

std::string WinPath::getCWDA() {
	std::string ret;
	DWORD reqlen = 0;
	std::shared_ptr<char> buf(nullptr);
	
	reqlen = GetCurrentDirectoryA(0, NULL);
	buf = std::shared_ptr<char>(new char[reqlen], std::default_delete<char[]>());
	GetCurrentDirectoryA(reqlen, buf.get());
	
	ret = buf.get();
	
	return ret;
}

std::wstring WinPath::getCWDW() {
	std::wstring ret;
	DWORD reqlen = 0;
	std::shared_ptr<WCHAR> buf(nullptr);
	
	reqlen = GetCurrentDirectoryW(0, NULL);
	buf = std::shared_ptr<WCHAR>(new WCHAR[reqlen], std::default_delete<WCHAR[]>());
	GetCurrentDirectoryW(reqlen, buf.get());
	
	ret = buf.get();
	
	return ret;
}

bool WinPath::setCWDA(std::string& path) {
	bool res = false;
	do {
		if (!isDirA(path)) {
			dprintf("[WinPath::setCWDA] %s is not a directory", path.c_str());
			break;
		}
		if (!SetCurrentDirectoryA(path.c_str())) {
			dprintf("[WinPath::setCWDA] SetCurrentDirectoryA(%s) failed: 0x%08x", GetLastError());
			break;
		}
		res = true;
	} while (0);

	return res;
}

bool WinPath::setCWDW(std::wstring& path) {
	bool res = false;

	do {
		if (!isDirW(path)) {
			dprintf("[WinPath::setCWDW] %S is not a directory", path.c_str());
			break;
		}
		if (!SetCurrentDirectoryW(path.c_str())) {
			dprintf("[WinPath::setCWDW] SetCurrentDirectoryW(%S) failed : 0x%08x", GetLastError());
			break;
		}
		res = true;
	} while (0);

	return res;
}

bool WinPath::getAbsPathA(std::string& in, std::string& out) {
	DWORD char_cnt = 0;
	DWORD copycnt = 0;
	std::shared_ptr<char> buf(nullptr);
	char* filepart = nullptr;
	bool res = false;
	do {
		if ((char_cnt = GetFullPathNameA(in.c_str(), 0, nullptr, &filepart)) == 0) {
			dprintf("[WinPath::getAbsPathA] GetFullPathNameA(%s) failed to get char_cnt: 0x%08x", in.c_str(), GetLastError());
			break;
		}

		buf = std::shared_ptr<char>(new char[char_cnt], std::default_delete<char[]>());

		if ((copycnt = GetFullPathNameA(in.c_str(), char_cnt, buf.get(), &filepart)) == 0) {
			dprintf("[WinPath::getAbsPathA] GetFullPathNameA(%s) failed : 0x%08x", in.c_str(), GetLastError());
			break;
		}

		out = buf.get();
		res = true;
	} while (0);

	return res;
}

bool WinPath::getAbsPathW(std::wstring& in, std::wstring& out) {
	DWORD wchar_cnt = 0;
	DWORD copycnt = 0;
	std::shared_ptr<WCHAR> wbuf(nullptr);
	WCHAR* filepart = nullptr;
	bool res = false;
	do {
		if ((wchar_cnt = GetFullPathNameW(in.c_str(), 0, nullptr, &filepart)) == 0) {
			dprintf("[WinPath::getAbsPathW] GetFullPathNameW(%S) failed to get wchar_cnt: 0x%08x", in.c_str(), GetLastError());
			break;
		}

		wbuf = std::shared_ptr<WCHAR>(new WCHAR[wchar_cnt], std::default_delete<WCHAR[]>());

		if ((copycnt = GetFullPathNameW(in.c_str(), wchar_cnt, wbuf.get(), &filepart)) == 0) {
			dprintf("[WinPath::getAbsPathW] GetFullPathNameW(%S) failed : 0x%08x", in.c_str(), GetLastError());
			break;
		}

		out = wbuf.get();
		res = true;
	} while (0);

	return res;
}

bool WinPath::listDirA(std::string& dir, std::vector<std::string>& res) {
	bool success = false;
	HANDLE hfind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA ffd;

	do {
		if (!WinPath::isDirA(dir)) {
			dprintf("[WinPath::listDirA] %s is not a directory", dir.c_str());
			break;
		}

		dir += (dir[dir.size() - 1] == '\\') ? "*" : "\\*";

		if ((hfind = FindFirstFileA(dir.c_str(), &ffd)) == INVALID_HANDLE_VALUE) {
			dprintf("[WinPath::listDirA] FindFirstFileA error : 0x%08x", GetLastError());
			break;
		}

		res.clear();
		do {
			res.push_back(ffd.cFileName);
		} while (FindNextFileA(hfind, &ffd) != 0);

		FindClose(hfind);
		success = true;
	} while (0);

	return success;
}

bool WinPath::listDirW(std::wstring& dir, std::vector<std::wstring>& res) {
	bool success = false;
	HANDLE hfind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAW ffd;

	do {
		if (!WinPath::isDirW(dir)) {
			dprintf("[WinPath::listDirW] %S is not a directory", dir.c_str());
			break;
		}

		dir += (dir[dir.size() - 1] == L'\\') ? L"*" : L"\\*";
		
		if ((hfind = FindFirstFileW(dir.c_str(), &ffd)) == INVALID_HANDLE_VALUE) {
			dprintf("[WinPath::listDirW] FindFirstFileW error : 0x%08x", GetLastError());
			break;
		}

		res.clear();
		do {
			res.push_back(ffd.cFileName);
		} while (FindNextFileW(hfind, &ffd) != 0);

		FindClose(hfind);
		success = true;
	} while (0);

	return success;
}

WinFile* WinPath::openFileA(std::string& path, std::string mode) {
	std::wstring wpath(path.begin(), path.end());
	std::wstring wmode(mode.begin(), mode.end());
	return openFileW(wpath, wmode);
}

WinFile* WinPath::openFileW(std::wstring& path, std::wstring mode) {
	WinFile* new_winfile = nullptr;
	HANDLE hfile = INVALID_HANDLE_VALUE;
	std::wstring abspath;

	DWORD desired_access = 0;
	DWORD share_mode = 0;
	DWORD creation_disposition = 0;

	do {
		if (mode == L"r") {
			desired_access = GENERIC_READ; 
			share_mode = FILE_SHARE_READ; 
			creation_disposition = OPEN_EXISTING;
		}
		else if (mode == L"r+") {
			desired_access = GENERIC_READ | GENERIC_WRITE; 
			creation_disposition = OPEN_EXISTING;
		}
		else if (mode == L"w") {
			desired_access = GENERIC_WRITE; 
			creation_disposition = CREATE_ALWAYS;
		}
		else if (mode == L"w+") {
			desired_access = GENERIC_READ | GENERIC_WRITE; 
			creation_disposition = CREATE_ALWAYS;
		}
		else if (mode == L"a") {
			desired_access = GENERIC_WRITE; 
			creation_disposition = OPEN_ALWAYS;
		}
		else if (mode == L"a+") {
			desired_access = GENERIC_READ | GENERIC_WRITE; 
			creation_disposition = OPEN_ALWAYS;
		}
		else if (mode == L"rw") { // custom
			desired_access = GENERIC_READ | GENERIC_WRITE; 
			creation_disposition = OPEN_ALWAYS;
		}
		else {
			dprintf("[WinPath::openFileW] Invalid open mode: %S", mode.c_str());
			break;
		}

		if ((hfile = CreateFileW(
						path.c_str(), 
						desired_access, 
						share_mode, 
						NULL, 
						creation_disposition, 
						FILE_ATTRIBUTE_NORMAL, 
						NULL
		)) == INVALID_HANDLE_VALUE) {
			dprintf("[WinPath::openFileW] CreateFileW (%S,%S) failed : 0x%08X", path.c_str(), mode.c_str(), GetLastError());
			break;
		}
	} while (0);

	if (hfile != INVALID_HANDLE_VALUE) {
		new_winfile = new WinFile(hfile, mode);
		if (mode[0] == L'a') {
			if (!(new_winfile->setPosToEnd())) {
				dprintf("[WinPath::openFileW] failed to set pointer at end for mode: %S)", mode.c_str());
				delete new_winfile;
				new_winfile = nullptr;
			}
		}
	}

	return new_winfile;
}

bool WinPath::moveFileA(std::string& src, std::string& dst, bool overwrite) {
	bool result = false;
	std::wstring w_src(src.begin(), src.end());
	std::wstring w_dst(dst.begin(), dst.end());
	if (!(result = moveFileW(w_src, w_dst, overwrite))) {
		dprintf("[WinPath::moveFileA] Failed");
	}
	return result;
}

bool WinPath::moveFileW(std::wstring& src, std::wstring& dst, bool overwrite) {
	bool result = false;
	DWORD move_flag = MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH;
	
	do {
		if (overwrite) {
			move_flag |= MOVEFILE_REPLACE_EXISTING;
		}
		if (!MoveFileExW(src.c_str(), dst.c_str(), move_flag)) {
			dprintf("[WinPath::moveFileW] MoveFileExW failed : 0x%08x", GetLastError());
			break;
		}
		result = true;
	} while (0);

	return result;
}