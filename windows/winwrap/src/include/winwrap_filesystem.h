#pragma once

#include <string>
#include <vector>

#include <Windows.h>

class WinPath {
public:
	static bool is_fileA(std::string& path);
	static bool is_fileW(std::wstring& path);
	static bool is_dirA(std::string& path);
	static bool is_dirW(std::wstring& path);

	static std::string cwdA();
	static std::wstring cwdW();

	static bool listdirW(std::wstring& dir, std::vector<std::wstring>& res);
	static bool listdirA(std::string& dir, std::vector<std::string>& res);

private:
	WinPath();
};