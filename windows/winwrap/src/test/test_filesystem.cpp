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
				printf("(%u)", w.size());
				wcout << w;
				printf("\n");
			}
			printf("%lu files\n", res.size());
		}
	}
}