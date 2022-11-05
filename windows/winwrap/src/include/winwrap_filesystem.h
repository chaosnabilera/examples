#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Windows.h>

class WinFile;

class WinPath {
public:
	static bool isPathA(std::string& path);
	static bool isPathW(std::wstring& path);
	
	static bool isFileA(std::string& path);
	static bool isFileW(std::wstring& path);
	
	static bool isDirA(std::string& path);
	static bool isDirW(std::wstring& path);

	static bool getFileTimeA(std::string& path, FILETIME* creation_time, FILETIME* last_access_time, FILETIME* last_write_time);
	static bool getFileTimeW(std::wstring& path, FILETIME* creation_time, FILETIME* last_access_time, FILETIME* last_write_time);

	static bool getFileTimeAsSystemTimeA(std::string& path, SYSTEMTIME* creation_time, SYSTEMTIME* last_access_time, SYSTEMTIME* last_write_time);
	static bool getFileTimeAsSystemTimeW(std::wstring& path, SYSTEMTIME* creation_time, SYSTEMTIME* last_access_time, SYSTEMTIME* last_write_time);

	static std::string getCWDA();
	static std::wstring getCWDW();

	static bool setCWDA(std::string& path);
	static bool setCWDW(std::wstring& path);

	static bool getAbsPathA(std::string& in, std::string* out);
	static bool getAbsPathW(std::wstring& in, std::wstring* out);

	static bool listDirA(std::string& dir, std::vector<std::string>* out_list);
	static bool listDirW(std::wstring& dir, std::vector<std::wstring>* out_list);

	static WinFile* openFileA(std::string& path, std::string mode);
	static WinFile* openFileW(std::wstring& path, std::wstring mode);

	static bool moveFileA(std::string& src, std::string& dst, bool overwrite = false);
	static bool moveFileW(std::wstring& src, std::wstring& dst, bool overwrite = false);

	// return true if directory creation success or directory already exist
	// supports nested directory creation
	static bool createDirA(std::string& path);
	static bool createDirW(std::wstring& path);

	// return true if deletion success or path does not exist
	static bool deleteFileA(std::string& path);
	static bool deleteFileW(std::wstring& path);

	//
	// shellXXXXX functions use SHFileOperation
	//
	static std::shared_ptr<WCHAR> toPCZZWSTR(std::wstring& path);
	static std::shared_ptr<WCHAR> toPCZZWSTR(std::vector<std::wstring>& pathlist);

	static bool shellDeletePathA(std::string& path, bool use_recycle_bin = false);
	static bool shellDeletePathW(std::wstring& path, bool use_recycle_bin = false);

	static bool shellMovePathA(std::string& src, std::string& dst, bool overwrite = false);
	static bool shellMovePathW(std::wstring& src, std::wstring& dst, bool overwrite = false);

	static bool shellCopyPathA(std::string& src, std::string& dst, bool overwrite = false);
	static bool shellCopyPathW(std::wstring& src, std::wstring& dst, bool overwrite = false);
	
	static bool shellRenamePathA(std::string& src, std::string& dst, bool overwrite = false);
	static bool shellRenamePathW(std::wstring& src, std::wstring& dst, bool overwrite = false);
	
private:
	WinPath();
};

class WinFile {
public:
	friend class WinPath;
	~WinFile();
	bool fileSize(long long* out_filesize);
	bool getPos(long long* out_pos);
	bool setPos(long long abspos);
	bool setPosToBeg();
	bool setPosToEnd();
	bool readBytes(BYTE* outbuf, long long outbuf_len, long long readlen, long long* out_readcnt);
	bool writeBytes(BYTE* inbuf, long long inbuf_len);
	bool readAll(std::shared_ptr<BYTE>* out_buf, long long* out_buflen);
private:
	WinFile(HANDLE hfile, std::wstring& mode);
	HANDLE hFile;
	std::wstring& mode;
};