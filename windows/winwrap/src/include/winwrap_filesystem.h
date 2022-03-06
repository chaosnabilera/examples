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

	static std::string get_cwdA();
	static std::wstring get_cwdW();

	static bool set_cwdA(std::string& path);
	static bool set_cwdW(std::wstring& path);

	static bool listdirW(std::wstring& dir, std::vector<std::wstring>& res);
	static bool listdirA(std::string& dir, std::vector<std::string>& res);

	static bool get_abspathW(std::wstring& in, std::wstring& out);
	static bool get_abspathA(std::string& in, std::string& out);

private:
	WinPath();
};