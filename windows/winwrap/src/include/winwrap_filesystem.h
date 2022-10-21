#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Windows.h>

class WinFile;

class WinPath {
public:
	static bool isFileA(std::string& path);
	static bool isFileW(std::wstring& path);
	static bool isDirA(std::string& path);
	static bool isDirW(std::wstring& path);

	static std::string getCWDA();
	static std::wstring getCWDW();

	static bool setCWDA(std::string& path);
	static bool setCWDW(std::wstring& path);

	static bool getAbsPathA(std::string& in, std::string& out);
	static bool getAbsPathW(std::wstring& in, std::wstring& out);

	static bool listDirA(std::string& dir, std::vector<std::string>& res);
	static bool listDirW(std::wstring& dir, std::vector<std::wstring>& res);

	static WinFile* openFileA(std::string& path, std::string mode);
	static WinFile* openFileW(std::wstring& path, std::wstring mode);

private:
	WinPath();
};

class WinFile {
public:
	friend class WinPath;
	~WinFile();
	bool fileSize(long long& out_filesize);
	bool getPos(long long& out_pos);
	bool setPos(long long abspos);
	bool setPosToBeg();
	bool setPosToEnd();
	bool readBytes(BYTE* outbuf, long long outbuf_len, long long readlen, long long& resultcnt);
	bool writeBytes(BYTE* inbuf, long long inbuf_len);
	bool readAll(std::pair<std::shared_ptr<BYTE>, long long>& out_pair);
private:
	WinFile(HANDLE hfile, std::wstring& mode);
	HANDLE hFile;
	std::wstring& mode;
};