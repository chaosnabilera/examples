#include <iostream>
#include <string>

#include <locale.h>

#include "winwrap_encoding.h"
#include "winwrap_filesystem.h"

static bool WWPrintCurrentWorkingDirectory();

void WWNavigateFileSystem() {
	char mbline[0x1000] = { 0 };
	WCHAR wcline[0x1000] = { 0 };
	int assign_cnt = 0;
	std::wstring icmd, iarg;
	

	printf("Shell!\n");

	while (true) {
		printf("%S>", WinPath::getCWDW().c_str());
		assign_cnt = scanf("%s", mbline);
		WCHAR* tmp_wcline = WinEncoding::utf8_to_wchar(mbline);
		icmd = tmp_wcline;
		delete[] tmp_wcline;
		// std::wcout << iline << std::endl;
		
		if (icmd == L"exit") {
			break;
		}
		else if (icmd == L"cd") {
			assign_cnt = scanf("%s", mbline);
			WCHAR* tmp_wcline = WinEncoding::utf8_to_wchar(mbline);
			iarg = tmp_wcline;
			delete[] tmp_wcline;
			
			if (!WinPath::setCWDW(iarg)) {
				printf("WinPath::setCWDW(%S) failed\n", iarg.c_str());
				continue;
			}
		}
		else if (icmd == L"cwd") {
			printf("%S\n", WinPath::getCWDW().c_str());
		}
		else if (icmd == L"ls") {
			if (!WWPrintCurrentWorkingDirectory()) {
				printf("[WWNavigateFileSystem] WWPrintCurrentWorkingDirectory failed!\n");
			}
		}
	}
}

bool WWPrintCurrentWorkingDirectory() {
	bool result = false;
	bool pathlist_result = false;
	std::vector<std::wstring> pathlist;
	std::wstring cur = L".";
	std::wstring cur_abspath;
	std::wstring sub_abspath;
	std::wstring is_dir;
	std::wstring is_file;
	SYSTEMTIME creation_time = { 0 };
	SYSTEMTIME last_access_time = { 0 };
	SYSTEMTIME last_write_time = { 0 };
	
	do {
		if (!WinPath::getAbsPathW(cur, cur_abspath)) {
			printf("[WWPrintCurrentWorkingDirectory] getAbsPathW current working directory failed\n");
			break;
		}
		if (!WinPath::listDirW(cur_abspath, pathlist)) {
			printf("[WWPrintCurrentWorkingDirectory] WinPath::listDirW(%S) failed\n", cur_abspath.c_str());
			break;
		}
		
		pathlist_result = true;
		for (std::wstring& sub : pathlist) {
			if (!WinPath::getAbsPathW(sub, sub_abspath)) {
				printf("[WWPrintCurrentWorkingDirectory] WinPath::getAbsPathW %S failed\n", sub.c_str());
				break;
			}
			
			is_dir = WinPath::isDirW(sub_abspath) ? L"dir" : L"";
			is_file = WinPath::isFileW(sub_abspath) ? L"file" : L"";

			if (WinPath::getFileTimeAsSystemTimeW(sub_abspath, &creation_time, &last_access_time, &last_write_time)) {
				/*
				printf("%S %S %S %S ct: %04d-%02d-%02d %02d:%02d:%02d at: %04d-%02d-%02d %02d:%02d:%02d wt : %04d-%02d-%02d %02d:%02d:%02d\n",
					sub.c_str(), sub_abspath.c_str(), is_dir.c_str(), is_file.c_str(),
					creation_time.wYear, creation_time.wMonth, creation_time.wDay, creation_time.wHour, creation_time.wMinute, creation_time.wSecond,
					last_access_time.wYear, last_access_time.wMonth, last_access_time.wDay, last_access_time.wHour, last_access_time.wMinute, last_access_time.wSecond,
					last_write_time.wYear, last_write_time.wMonth, last_write_time.wDay, last_write_time.wHour, last_write_time.wMinute, last_write_time.wSecond);
				*/
				printf("%-50S %3S wt : %04d-%02d-%02d %02d:%02d:%02d\n", sub.c_str(), is_dir.c_str(), last_write_time.wYear, last_write_time.wMonth, last_write_time.wDay, last_write_time.wHour, last_write_time.wMinute, last_write_time.wSecond);
			}
			else {
				// printf("%S %S %S %S (file time access failed)\n", sub.c_str(), sub_abspath.c_str(), is_dir.c_str(), is_file.c_str());
				printf("%-50S %3S wt : access denied\n", is_dir.c_str(), sub.c_str());
			}			
		}
		
		if (!pathlist_result) {
			break;
		}
		
		result = true;
	} while (0);

	return result;
	
}

void WWCopyFileSimple(std::string src, std::string dst) {
	WinFile* ifile = nullptr;
	WinFile* ofile = nullptr;
	std::pair<std::shared_ptr<BYTE>, long long> ifile_content;
	BYTE* ifile_buf = nullptr;
	long long ifile_buf_len = 0;
	

	do {
		if (!WinPath::isFileA(src)) {
			printf("%s is not a file!\n", src.c_str());
			break;
		}
		if ((ifile = WinPath::openFileA(src, "r")) == nullptr) {
			printf("failed to open %s for read\n", src.c_str());
			break;
		}

		if (!ifile->readAll(ifile_content)) {
			printf("failed to read_all %s\n", src.c_str());
			break;
		}
		
		ifile_buf = ifile_content.first.get();
		ifile_buf_len = ifile_content.second;

		printf("size of %s: %lld\n", src.c_str(), ifile_buf_len);

		if ((ofile = WinPath::openFileA(dst, "w")) == nullptr) {
			printf("failed to open %s for write\n", src.c_str());
			break;
		}

		if (!(ofile->writeBytes(ifile_buf, ifile_buf_len))) {
			printf("failed to write to %s\n", dst.c_str());
			break;
		}

		printf("copy %s -> %s success!\n", src.c_str(), dst.c_str());
	} while (0);

	if (ifile)
		delete ifile;
	if (ofile)
		delete ofile;
}