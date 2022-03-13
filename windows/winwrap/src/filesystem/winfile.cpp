#include "winwrap_filesystem.h"

#include "dprintf.hpp"

using namespace std;

WinFile::WinFile(HANDLE _hfile, std::wstring& _mode) : hfile(_hfile), mode(_mode) {

}

WinFile::~WinFile() {
	CloseHandle(hfile);
}

bool WinFile::filesize(long long& out_filesize) {
	bool res;
	LARGE_INTEGER li;
	if ((res = GetFileSizeEx(hfile, &li))) {
		out_filesize = li.QuadPart;
	}
	return res;
}

bool WinFile::getpos(long long& out_pos) {
	bool res;
	LARGE_INTEGER li_zero = { 0 };
	LARGE_INTEGER li_pos;
	if ((res = SetFilePointerEx(hfile, li_zero, &li_pos, FILE_CURRENT))) {
		out_pos = li_pos.QuadPart;
	}
	return res;
}

bool WinFile::setpos(long long abspos) {
	LARGE_INTEGER li_target;
	LARGE_INTEGER li_pos;
	li_target.QuadPart = abspos;
	return SetFilePointerEx(hfile, li_target, &li_pos, FILE_BEGIN);
}

bool WinFile::setpos_beg() {
	return setpos(0);
}

bool WinFile::setpos_end() {
	LARGE_INTEGER li_zero = { 0 };
	LARGE_INTEGER li_pos;
	return SetFilePointerEx(hfile, li_zero, &li_pos, FILE_END);
}

bool WinFile::read(BYTE* outbuf, long long outbuf_len, long long readlen, long long& resultcnt) {
	bool success = false;
	long long bytesread = 0;
	DWORD error = ERROR_SUCCESS;

	resultcnt = 0;
	do {
		if (outbuf_len < readlen) {
			dprintf("[WinFile::read] outbuf_len(%lld) < readlen(%lld)", outbuf_len, readlen);
			break;
		}

		while (bytesread < readlen) {
			DWORD dtoread = ((readlen - bytesread) > 0xFFFFFFFF) ? 0xFFFFFFFF : readlen - bytesread;
			DWORD dreadcnt = 0;
			if (!ReadFile(hfile, outbuf + bytesread, dtoread, &dreadcnt, nullptr)) {
				error = GetLastError();
				dprintf("[WinFile::read] ReadFile failed with %d(0x%08X)", error, error);
				break;
			}
			if (dreadcnt == 0) // end of file reached
				break;
			bytesread += dreadcnt;
		}
		if (error != ERROR_SUCCESS) {
			break;
		}
		resultcnt = bytesread;
		success = true;
	} while (0);

	return success;
}

bool WinFile::write(BYTE* inbuf, long long inbuf_len) {
	bool success = false;
	long long byteswritten = 0;
	DWORD error = ERROR_SUCCESS;

	do {
		while (byteswritten < inbuf_len) {
			DWORD dtowrite = (inbuf_len - byteswritten > 0xFFFFFFFF) ? 0xFFFFFFFF : inbuf_len - byteswritten;
			DWORD dwritten = 0;
			if (!WriteFile(hfile, inbuf + byteswritten, dtowrite, &dwritten, nullptr)) {
				error = GetLastError();
				dprintf("[WinFile::write] WriteFile failed with %d(0x%08X)", error, error);
				break;
			}
			byteswritten += dwritten;
		}
		if (error != ERROR_SUCCESS)
			break;
		success = true;
	} while (0);

	return success;
}

shared_ptr<BYTE> WinFile::read_all(long long& resultcnt) {
	bool success = false;
	long long cur_filesize = 0;
	BYTE* newbuf = nullptr;
	shared_ptr<BYTE> result;

	long long curpos = 0;
	long long read_result = 0;

	do {
		if (!filesize(cur_filesize)) {
			dprintf("[WinFile::read_all] failed to get filesize");
			break;
		}
		if ((newbuf = (BYTE*)malloc(cur_filesize)) == NULL) {
			dprintf("[WinFile::read_all] failed malloc %lld bytes. not enough memory", cur_filesize);
			break;
		}

		result = shared_ptr<BYTE>(newbuf, free);

		if (!getpos(curpos)) {
			dprintf("[WinFile::read_all] failed to get current position");
			break;
		}
		if (!setpos_beg()) {
			dprintf("[WinFile::read_all] failed to set position to beginning");
			break;
		}
		if (!read(newbuf, cur_filesize, cur_filesize, read_result)) {
			dprintf("[WinFile::read_all] read %lld bytes failed", cur_filesize);
		}
		if (!setpos(curpos)) {
			dprintf("[WinFile::read_all] failed to restore file position");
			break;
		}
		resultcnt = cur_filesize;
		success = true;
	} while (0);

	if (!success) {
		result.reset();
	}

	return result;
}