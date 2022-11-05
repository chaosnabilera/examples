#include <windows.h>

#include <cstring>
#include <memory>

#include "dprintf.hpp"
#include "winwrap_filesystem.h"

static bool shellSrcDstCommonOperationA(UINT op, std::string& src, std::string& dst, bool overwrite);
static bool shellSrcDstCommonOperationW(UINT op, std::wstring& src, std::wstring& dst, bool overwrite);


bool WinPath::isPathA(std::string& path) {
	std::wstring wpath(path.begin(), path.end());
	return isPathW(wpath);
}

bool WinPath::isPathW(std::wstring& path) {
	return (GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES);
}

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

bool WinPath::getAbsPathA(std::string& in, std::string* out) {
	DWORD char_cnt = 0;
	DWORD copycnt = 0;
	std::shared_ptr<char> buf(nullptr);
	char* filepart = nullptr; // filepart does not need extra buffer. it points to a location in resulting full path
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

		*out = buf.get();
		res = true;
	} while (0);

	return res;
}

bool WinPath::getAbsPathW(std::wstring& in, std::wstring* out) {
	DWORD wchar_cnt = 0;
	DWORD copycnt = 0;
	std::shared_ptr<WCHAR> wbuf(nullptr);
	WCHAR* filepart = nullptr; // filepart does not need extra buffer. it points to a location in resulting full path
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

		*out = wbuf.get();
		res = true;
	} while (0);

	return res;
}

bool WinPath::listDirA(std::string& dir, std::vector<std::string>* res) {
	bool success = false;
	HANDLE hfind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA ffd;

	do {
		if (!isDirA(dir)) {
			dprintf("[WinPath::listDirA] %s is not a directory", dir.c_str());
			break;
		}

		dir += (dir[dir.size() - 1] == '\\') ? "*" : "\\*";

		if ((hfind = FindFirstFileA(dir.c_str(), &ffd)) == INVALID_HANDLE_VALUE) {
			dprintf("[WinPath::listDirA] FindFirstFileA error : 0x%08x", GetLastError());
			break;
		}

		res->clear();
		do {
			res->push_back(ffd.cFileName);
		} while (FindNextFileA(hfind, &ffd) != 0);

		FindClose(hfind);
		success = true;
	} while (0);

	return success;
}

bool WinPath::listDirW(std::wstring& dir, std::vector<std::wstring>* res) {
	bool success = false;
	HANDLE hfind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAW ffd;

	do {
		if (!isDirW(dir)) {
			dprintf("[WinPath::listDirW] %S is not a directory", dir.c_str());
			break;
		}

		dir += (dir[dir.size() - 1] == L'\\') ? L"*" : L"\\*";
		
		if ((hfind = FindFirstFileW(dir.c_str(), &ffd)) == INVALID_HANDLE_VALUE) {
			dprintf("[WinPath::listDirW] FindFirstFileW error : 0x%08x", GetLastError());
			break;
		}

		res->clear();
		do {
			res->push_back(ffd.cFileName);
		} while (FindNextFileW(hfind, &ffd) != 0);

		FindClose(hfind);
		success = true;
	} while (0);

	return success;
}

bool WinPath::openFileA(std::string& path, std::string mode, std::shared_ptr<WinFile>* out_winfile) {
	bool result = false;
	std::wstring wpath(path.begin(), path.end());
	std::wstring wmode(mode.begin(), mode.end());

	if (!(result = openFileW(wpath, wmode, out_winfile))) {
		dprintf("[WinPath::openFileA] Failed");
	}
	
	return result;
}

bool WinPath::openFileW(std::wstring& path, std::wstring mode, std::shared_ptr<WinFile>* out_winfile) {
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
		if (new_winfile) {
			*out_winfile = std::shared_ptr<WinFile>(new_winfile);
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

bool WinPath::createDirA(std::string& path) {
	std::wstring wpath(path.begin(), path.end());
	bool result = false;
	if (!(result = createDirW(wpath))) {
		dprintf("[WinPath::createDirA] Failed");
	}
	return result;
}

bool WinPath::createDirW(std::wstring& path) {
	std::wstring abspath;

	if (!getAbsPathW(path, &abspath)) {
		dprintf("[WinPath::createDirW] getAbsPathW failed : %S", path.c_str());
		return false;
	}
	
	if (isFileW(abspath)) {
		dprintf("[WinPath::createDirW] File with same name exists : %S", path.c_str());
		return false;
	}

	if (isDirW(abspath))
		return true;

	for (size_t i = abspath.length() - 1; i >= 0; --i) {
		if (abspath[i] == L'\\' || abspath[i] == L'/') {
			std::wstring subpath = abspath.substr(0, i);
			if (createDirW(subpath)) {
				break;
			}
			else {
				dprintf("[WinPath::createDirW] creation of subdirectory %S failed", subpath.c_str());
				return false;
			}
		}
	}
	
	if (CreateDirectoryW(abspath.c_str(), NULL)) {
		return true;
	}

	dprintf("[WinPath::createDirW] CreateDirectoryW %S failed : 0x%08x", abspath.c_str(), GetLastError());
	return false;
}

bool WinPath::deleteFileA(std::string& path) {
	std::wstring wpath(path.begin(), path.end());
	bool result = false;
	if (!(result = deleteFileW(wpath))) {
		dprintf("[WinPath::deleteFileA] Failed");
	}
	return result;
}

bool WinPath::deleteFileW(std::wstring& path) {
	if (!DeleteFileW(path.c_str())) {
		dprintf("[WinPath::deleteFileW] DeleteFileW %S failed : 0x%08x", path.c_str(), GetLastError());
		return false;
	}	
	return true;
}

std::shared_ptr<WCHAR> WinPath::toPCZZWSTR(std::wstring& path) {
	std::shared_ptr<WCHAR> result(nullptr);
	
	result = std::shared_ptr<WCHAR>(new WCHAR[path.length() + 2], std::default_delete<WCHAR[]>());
	memcpy(result.get(), path.c_str(), sizeof(WCHAR) * path.length());
	result.get()[path.length()] = L'\0';
	result.get()[path.length() + 1] = L'\0';
	
	return result;
}

std::shared_ptr<WCHAR> WinPath::toPCZZWSTR(std::vector<std::wstring>& pathlist) {
	std::shared_ptr<WCHAR> result(nullptr);
	size_t total_length = 0;
	size_t offset = 0;

	for (auto& path : pathlist) {
		total_length += (path.length() + 2);
	}

	result = std::shared_ptr<WCHAR>(new WCHAR[total_length], std::default_delete<WCHAR[]>());
	
	for (auto& path : pathlist) {
		memcpy(result.get() + offset, path.c_str(), sizeof(WCHAR) * path.length());
		result.get()[offset + path.length()] = L'\0';
		result.get()[offset + path.length() + 1] = L'\0';
		offset += path.length() + 2;
	}

	return result;
}

bool WinPath::shellDeletePathA(std::string& path, bool use_recycle_bin) {
	std::wstring wpath(path.begin(), path.end());
	bool result = false;
	if (!(result = shellDeletePathW(wpath, use_recycle_bin))) {
		dprintf("[WinPath::shellDeleteFileA] Failed");
	}
	return result;
}

bool WinPath::shellDeletePathW(std::wstring& path, bool use_recycle_bin) {
	SHFILEOPSTRUCTW fileop = { 0 };
	std::shared_ptr<WCHAR> path_pczzwstr;
	int error_code = 0;

	path_pczzwstr = toPCZZWSTR(path);
	
	fileop.hwnd = NULL;
	fileop.wFunc = FO_DELETE;
	fileop.pFrom = path_pczzwstr.get();
	fileop.pTo = NULL;
	fileop.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	if (use_recycle_bin) {
		fileop.fFlags |= FOF_ALLOWUNDO;
	}
	if ((error_code = SHFileOperationW(&fileop)) != 0) {
		dprintf("[WinPath::shellDeletePathW] SHFileOperationW %S failed : 0x%08x", path.c_str(), error_code);
		return false;
	}
	if (fileop.fAnyOperationsAborted) {
		dprintf("[WinPath::shellDeletePathW] SHFileOperationW %S aborted", path.c_str());
		return false;
	}
	
	return true;
}

bool WinPath::shellMovePathA(std::string& src, std::string& dst, bool overwrite) {
	bool result = false;
	if (!(result = shellSrcDstCommonOperationA(FO_MOVE, src, dst, overwrite))) {
		dprintf("[WinPath::shellMovePathA] Failed");
	}
	return result;
	
}
bool WinPath::shellMovePathW(std::wstring& src, std::wstring& dst, bool overwrite) {
	bool result = false;
	if (!(result = shellSrcDstCommonOperationW(FO_MOVE, src, dst, overwrite))) {
		dprintf("[WinPath::shellMovePathW] Failed");
	}
	return result;
}

bool WinPath::shellCopyPathA(std::string& src, std::string& dst, bool overwrite) {
	bool result = false;
	if (!(result = shellSrcDstCommonOperationA(FO_COPY, src, dst, overwrite))) {
		dprintf("[WinPath::shellCopyPathA] Failed");
	}
	return result;
}

bool WinPath::shellCopyPathW(std::wstring& src, std::wstring& dst, bool overwrite) {
	bool result = false;
	if (!(result = shellSrcDstCommonOperationW(FO_COPY, src, dst, overwrite))) {
		dprintf("[WinPath::shellCopyPathW] Failed");
	}
	return result;
}

bool WinPath::shellRenamePathA(std::string& src, std::string& dst, bool overwrite) {
	bool result = false;
	if (!(result = shellSrcDstCommonOperationA(FO_RENAME, src, dst, overwrite))) {
		dprintf("[WinPath::shellRenamePathA] Failed");
	}
	return result;
}

bool WinPath::shellRenamePathW(std::wstring& src, std::wstring& dst, bool overwrite) {
	bool result = false;
	if (!(result = shellSrcDstCommonOperationW(FO_RENAME, src, dst, overwrite))) {
		dprintf("[WinPath::shellRenamePathA] Failed");
	}
	return result;
}

bool shellSrcDstCommonOperationA(UINT op, std::string& src, std::string& dst, bool overwrite) {
	std::wstring wsrc(src.begin(), src.end());
	std::wstring wdst(dst.begin(), dst.end());

	return shellSrcDstCommonOperationW(op, wsrc, wdst, overwrite);
}

bool shellSrcDstCommonOperationW(UINT op, std::wstring& src, std::wstring& dst, bool overwrite) {
	SHFILEOPSTRUCTW fileop = { 0 };
	std::shared_ptr<WCHAR> src_pczzwstr;
	std::shared_ptr<WCHAR> dst_pczzwstr;
	int error_code = 0;

	src_pczzwstr = WinPath::toPCZZWSTR(src);
	dst_pczzwstr = WinPath::toPCZZWSTR(dst);

	fileop.hwnd = NULL;
	fileop.wFunc = op;
	fileop.pFrom = src_pczzwstr.get();
	fileop.pTo = dst_pczzwstr.get();
	fileop.fFlags = FOF_NOERRORUI | FOF_SILENT | FOF_NOCONFIRMMKDIR;
	fileop.fFlags |= FOF_NOCONFIRMATION;
	if (overwrite) {
		fileop.fFlags |= FOF_NOCONFIRMATION;
	}

	if ((error_code = SHFileOperationW(&fileop)) != 0) {
		dprintf("[shellSrcDstCommonOperationW] SHFileOperationW %S -> %S failed : 0x%08x", src.c_str(), dst.c_str(), error_code);
		return false;
	}
	if (fileop.fAnyOperationsAborted) {
		dprintf("[shellSrcDstCommonOperationW] SHFileOperationW %S -> %S aborted", src.c_str(), dst.c_str());
		return false;
	}
	return true;
}