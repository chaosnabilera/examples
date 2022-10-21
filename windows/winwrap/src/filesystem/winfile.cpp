#include "winwrap_filesystem.h"

#include "dprintf.hpp"

using namespace std;

WinFile::WinFile(HANDLE _hfile, std::wstring& _mode) : hFile(_hfile), mode(_mode) {

}

WinFile::~WinFile() {
	CloseHandle(hFile);
}

bool WinFile::fileSize(long long& out_filesize) {
	bool res;
	LARGE_INTEGER li;
	if ((res = GetFileSizeEx(hFile, &li))) {
		out_filesize = li.QuadPart;
	}
	return res;
}

bool WinFile::getPos(long long& out_pos) {
	bool res;
	LARGE_INTEGER li_zero = { 0 };
	LARGE_INTEGER li_pos;
	if ((res = SetFilePointerEx(hFile, li_zero, &li_pos, FILE_CURRENT))) {
		out_pos = li_pos.QuadPart;
	}
	return res;
}

bool WinFile::setPos(long long abspos) {
	LARGE_INTEGER li_target;
	LARGE_INTEGER li_pos;
	li_target.QuadPart = abspos;
	return SetFilePointerEx(hFile, li_target, &li_pos, FILE_BEGIN);
}

bool WinFile::setPosToBeg() {
	return setPos(0);
}

bool WinFile::setPosToEnd() {
	LARGE_INTEGER li_zero = { 0 };
	LARGE_INTEGER li_pos;
	return SetFilePointerEx(hFile, li_zero, &li_pos, FILE_END);
}

bool WinFile::readBytes(BYTE* outbuf, long long outbuf_len, long long readlen, long long& resultcnt) {
	bool success = false;
	long long bytes_read = 0;
	DWORD error = ERROR_SUCCESS;
	DWORD to_read = 0;
	DWORD did_read = 0;

	resultcnt = 0;
	do {
		if (outbuf_len < readlen) {
			dprintf("[WinFile::read] outbuf_len(%lld) < readlen(%lld)", outbuf_len, readlen);
			break;
		}

		while (bytes_read < readlen) {
			// because ReadFile can read at most 0xFFFFFFFF at a time
			to_read = ((readlen - bytes_read) > 0xFFFFFFFF) ? 0xFFFFFFFF : readlen - bytes_read;
			did_read = 0;
			if (!ReadFile(hFile, outbuf + bytes_read, to_read, &did_read, nullptr)) {
				error = GetLastError();
				dprintf("[WinFile::read] ReadFile failed : 0x%08X", error);
				break;
			}
			if (did_read == 0) // end of file reached
				break;
			bytes_read += did_read;
		}
		if (error != ERROR_SUCCESS) {
			break;
		}
		resultcnt = bytes_read;
		success = true;
	} while (0);

	return success;
}

bool WinFile::writeBytes(BYTE* inbuf, long long inbuf_len) {
	bool success = false;
	long long bytes_written = 0;
	DWORD error = ERROR_SUCCESS;
	DWORD to_write = 0;
	DWORD did_write = 0;

	do {
		while (bytes_written < inbuf_len) {
			to_write = (inbuf_len - bytes_written > 0xFFFFFFFF) ? 0xFFFFFFFF : inbuf_len - bytes_written;
			did_write = 0;
			if (!WriteFile(hFile, inbuf + bytes_written, to_write, &did_write, nullptr)) {
				error = GetLastError();
				dprintf("[WinFile::write] WriteFile failed with %d(0x%08X)", error, error);
				break;
			}
			bytes_written += did_write;
		}
		if (error != ERROR_SUCCESS)
			break;
		success = true;
	} while (0);

	return success;
}

bool WinFile::readAll(std::pair<std::shared_ptr<BYTE>, long long>& out_pair) {
	bool result = false;
	long long cur_filesize = 0;
	BYTE* new_alloc = nullptr;
	shared_ptr<BYTE> buf(nullptr);

	long long curpos = 0;
	long long read_result = 0;

	do {
		if (!fileSize(cur_filesize)) {
			dprintf("[WinFile::readAll] failed to get filesize");
			break;
		}
		if ((new_alloc = (BYTE*)malloc(cur_filesize)) == NULL) {
			dprintf("[WinFile::readAll] failed malloc %lld bytes. not enough memory", cur_filesize);
			break;
		}

		buf = shared_ptr<BYTE>(new_alloc, free);

		if (!getPos(curpos)) {
			dprintf("[WinFile::readAll] failed to get current position");
			break;
		}
		if (!setPosToBeg()) {
			dprintf("[WinFile::readAll] failed to set position to beginning");
			break;
		}
		if (!readBytes(new_alloc, cur_filesize, cur_filesize, read_result)) {
			dprintf("[WinFile::readAll] read %lld bytes failed", cur_filesize);
			break;
		}
		if (!setPos(curpos)) {
			dprintf("[WinFile::readAll] failed to restore file position");
			break;
		}
		
		out_pair.first = buf;
		out_pair.second = cur_filesize;
		result = true;
	} while (0);

	if (!result) {
		buf.reset();
	}

	return result;
}