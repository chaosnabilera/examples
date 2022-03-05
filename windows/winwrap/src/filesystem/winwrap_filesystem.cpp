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

string WinPath::cwdA() {
	DWORD reqlen = GetCurrentDirectoryA(0, NULL);
	char* wd = new char[reqlen];
	GetCurrentDirectoryA(reqlen, wd);
	string ret(wd);
	delete[] wd;
	return ret;
}

wstring WinPath::cwdW() {
	DWORD reqlen = GetCurrentDirectoryW(0, NULL);
	WCHAR* wd = new WCHAR[reqlen];
	GetCurrentDirectoryW(reqlen, wd);
	wstring ret(wd);
	delete[] wd;
	return ret;
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