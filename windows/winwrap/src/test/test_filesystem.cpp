#include <iostream>
#include <string>

#include <locale.h>

#include "winwrap_encoding.h"
#include "winwrap_filesystem.h"

using namespace std;

void navigate_filesystemW() {
	WCHAR iline[0x100];
	wstring icmd, iarg;

	printf("Shell!\n");

	while (true) {
		scanf("%S", iline);
		icmd = iline;
		if (icmd == L"exit") {
			break;
		}
		else if (icmd == L"cd") {
			scanf("%S", iline);
			iarg = iline;
			if (!WinPath::set_cwdW(iarg)) {
				printf("WinPath::set_cwdW(%S) failed\n", iarg.c_str());
				continue;
			}
		}
		else if (icmd == L"cwd") {
			printf("%S\n", WinPath::get_cwdW().c_str());
		}
		else if (icmd == L"ls") {
			vector<wstring> res;
			wstring cur = L".";
			if (!WinPath::listdirW(cur, res)) {
				printf("WinPath::listdirW(%S) failed\n", cur.c_str());
				continue;
			}
			for (auto& w : res) {
				printf("(%zu)", w.size());
				wcout << w;
				printf("\n");
			}
			printf("%zu files\n", res.size());
		}
	}
}

void copy_file(string src, string dst) {
	WinFile* ifile = nullptr;
	WinFile* ofile = nullptr;
	shared_ptr<BYTE> ifile_content;
	long long ifile_content_size = 0;

	do {
		if (!WinPath::is_fileA(src)) {
			printf("%s is not a file!\n", src.c_str());
			break;
		}
		if ((ifile = WinPath::open_fileA(src, "r")) == nullptr) {
			printf("failed to open %s for read\n", src.c_str());
			break;
		}

		ifile_content = ifile->read_all(ifile_content_size);
		if (!ifile_content) {
			printf("failed to read_all %s\n", src.c_str());
			break;
		}

		printf("size of %s: %lld\n", src.c_str(), ifile_content_size);

		if ((ofile = WinPath::open_fileA(dst, "w")) == nullptr) {
			printf("failed to open %s for write\n", src.c_str());
			break;
		}

		if (!(ofile->write(ifile_content.get(), ifile_content_size))) {
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