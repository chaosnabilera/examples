#include <iostream>
#include <string>

#include "winwrap_encoding.h"
#include "winwrap_filesystem.h"

using namespace std;

int main(int argc, char** argv) {
	string cwda = WinPath::cwdA();
	wstring cwdw = WinPath::cwdW();

	printf("%s\n%S\n", cwda.c_str(), cwdw.c_str());

	wstring dir = L"d:";
	vector<wstring> list;
	if (WinPath::listdirW(dir, list)) {
		for (auto& ws : list) {
			printf("%S\n", ws.c_str());
		}
	}

	return 0;
}