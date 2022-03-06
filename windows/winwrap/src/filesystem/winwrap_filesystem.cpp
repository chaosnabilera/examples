#include <cstring>
#include <windows.h>

#include "dprintf.hpp"
#include "winwrap_filesystem.h"

using namespace std;

bool WinPath::is_fileA(string& path) {
	wstring wpath(path.begin(), path.end());
	return is_fileW(wpath);
}

bool WinPath::is_fileW(wstring& path) {
	DWORD fattr = GetFileAttributesW(path.c_str());
	return (fattr != INVALID_FILE_ATTRIBUTES) && !(fattr & FILE_ATTRIBUTE_DIRECTORY);
}

bool WinPath::is_dirA(string& path) {
	wstring wpath(path.begin(), path.end());
	return is_dirW(wpath);
}

bool WinPath::is_dirW(wstring& path) {
	DWORD fattr = GetFileAttributesW(path.c_str());
	return (fattr != INVALID_FILE_ATTRIBUTES) && (fattr & FILE_ATTRIBUTE_DIRECTORY);
}

string WinPath::get_cwdA() {
	DWORD reqlen = GetCurrentDirectoryA(0, NULL);
	char* wd = new char[reqlen];
	GetCurrentDirectoryA(reqlen, wd);
	string ret(wd);
	delete[] wd;
	return ret;
}

wstring WinPath::get_cwdW() {
	DWORD reqlen = GetCurrentDirectoryW(0, NULL);
	WCHAR* wd = new WCHAR[reqlen];
	GetCurrentDirectoryW(reqlen, wd);
	wstring ret(wd);
	delete[] wd;
	return ret;
}

bool WinPath::set_cwdA(string& path) {
	bool res = false;
	do {
		if (!is_dirA(path)) {
			dprintf("%s is not a directory", path.c_str());
			break;
		}
		if (!SetCurrentDirectoryA(path.c_str())) {
			dprintf("SetCurrentDirectoryA(%s) failed: %d", GetLastError());
			break;
		}
		res = true;
	} while (0);

	return res;
}

bool WinPath::set_cwdW(wstring& path) {
	bool res = false;

	do {
		if (!is_dirW(path)) {
			dprintf("%S is not a directory", path.c_str());
			break;
		}
		if (!SetCurrentDirectoryW(path.c_str())) {
			dprintf("SetCurrentDirectoryW(%S) failed : %d", GetLastError());
			break;
		}
		res = true;
	} while (0);

	return res;
}

bool WinPath::listdirW(wstring& dir, vector<wstring>& res) {
	bool success = false;
	HANDLE hfind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAW ffd;

	do {
		if (!WinPath::is_dirW(dir)) {
			dprintf("%S is not a directory", dir.c_str());
			break;
		}

		dir += L"\\*";
		if ((hfind = FindFirstFileW(dir.c_str(), &ffd)) == INVALID_HANDLE_VALUE) {
			dprintf("FindFirstFileW error : GetLastError: %d", GetLastError());
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

bool WinPath::listdirA(string& dir, vector<string>& res) {
	bool success = false;
	HANDLE hfind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA ffd;

	do {
		if (!WinPath::is_dirA(dir)) {
			dprintf("%s is not a directory", dir.c_str());
			break;
		}

		dir += "\\*";
		if ((hfind = FindFirstFileA(dir.c_str(), &ffd)) == INVALID_HANDLE_VALUE) {
			dprintf("FindFirstFileA error : GetLastError: %d", GetLastError());
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

bool WinPath::get_abspathW(wstring& in, wstring& out) {
	DWORD bufsize = 0;
	DWORD copycnt = 0;
	WCHAR* wbuf = nullptr;
	WCHAR* filepart = nullptr;
	bool res = false;
	do {
		if ((bufsize = GetFullPathNameW(in.c_str(), 0, wbuf, &filepart)) == 0) {
			dprintf("GetFullPathNameW(%S) failed to get bufsize: %d", in.c_str(), GetLastError());
			break;
		}
		wbuf = new WCHAR[bufsize];
		if ((copycnt = GetFullPathNameW(in.c_str(), bufsize, wbuf, &filepart)) == 0) {
			dprintf("GetFullPathNameW(%S) failed : %d", in.c_str(), GetLastError());
			break;
		}
		out = wbuf;
		res = true;
	} while (0);

	if (wbuf)
		delete[] wbuf;

	return res;
}

bool WinPath::get_abspathA(string& in, string& out) {
	DWORD bufsize = 0;
	DWORD copycnt = 0;
	char* buf = nullptr;
	char* filepart = nullptr;
	bool res = false;
	do {
		if ((bufsize = GetFullPathNameA(in.c_str(), 0, buf, &filepart)) == 0) {
			dprintf("GetFullPathNameA(%s) failed to get bufsize: %d", in.c_str(), GetLastError());
			break;
		}
		buf = new char[bufsize];
		if ((copycnt = GetFullPathNameA(in.c_str(), bufsize, buf, &filepart)) == 0) {
			dprintf("GetFullPathNameA(%s) failed : %d", in.c_str(), GetLastError());
			break;
		}
		out = buf;
		res = true;
	} while (0);

	if (buf)
		delete[] buf;

	return res;
}