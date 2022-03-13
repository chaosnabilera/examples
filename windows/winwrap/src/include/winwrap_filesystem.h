#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Windows.h>

class WinFile;

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

	static WinFile* open_fileA(std::string& path, std::string mode);
	static WinFile* open_fileW(std::wstring& path, std::wstring mode);

private:
	WinPath();
};

class WinFile {
public:
	friend class WinPath;
	~WinFile();
	bool filesize(long long& out_filesize);
	bool getpos(long long& out_pos);
	bool setpos(long long abspos);
	bool setpos_beg();
	bool setpos_end();
	bool read(BYTE* outbuf, long long outbuf_len, long long readlen, long long& resultcnt);
	bool write(BYTE* inbuf, long long inbuf_len);
	std::shared_ptr<BYTE> read_all(long long& resultcnt);
private:
	WinFile(HANDLE hfile, std::wstring& mode);
	HANDLE hfile;
	std::wstring& mode;
};