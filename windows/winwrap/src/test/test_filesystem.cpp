#include <iostream>
#include <string>

#include <locale.h>

#include "winwrap_encoding.h"
#include "winwrap_filesystem.h"

void WWNavigateFileSystem() {
	WCHAR iline[0x100] = { 0 };
	int assign_cnt = 0;
	std::wstring icmd, iarg;
	

	printf("Shell!\n");

	while (true) {
		printf("%S>", WinPath::getCWDW().c_str());
		
		assign_cnt = scanf("%S", iline);
		
		icmd = iline;
		if (icmd == L"exit") {
			break;
		}
		else if (icmd == L"cd") {
			assign_cnt = scanf("%S", iline);
			
			iarg = iline;
			if (!WinPath::setCWDW(iarg)) {
				printf("WinPath::setCWDW(%S) failed\n", iarg.c_str());
				continue;
			}
		}
		else if (icmd == L"cwd") {
			printf("%S\n", WinPath::getCWDW().c_str());
		}
		else if (icmd == L"ls") {
			std::vector<std::wstring> res;
			std::wstring cur = L".";
			if (!WinPath::listDirW(cur, res)) {
				printf("WinPath::listDirW(%S) failed\n", cur.c_str());
				continue;
			}
			for (auto& w : res) {
				// printf("(%zu)", w.size());
				std::wcout << w;
				printf("\n");
			}
			printf("%zu files\n", res.size());
		}
	}
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