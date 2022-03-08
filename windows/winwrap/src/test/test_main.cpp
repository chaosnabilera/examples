#include <iostream>
#include <string>

#include <locale.h>

#include "winwrap_encoding.h"
#include "winwrap_filesystem.h"

//#include "dprintf.hpp"

using namespace std;

void navigate_filesystemW();
void run_echoclient(string server_addr, string server_port);
void run_echoserver(string server_addr, string server_port);

int main(int argc, char** argv) {
	vector<string> arg;

	// unicode characters won't be properly displayed if we omit these
	setlocale(LC_ALL, ".65001");    // for multi-byte
	_wsetlocale(LC_ALL, L".65001"); // for unicode

	for (int i = 0; i < argc; ++i)
		arg.push_back(argv[i]);

	if (arg.size() == 2 && arg[1] == "fs") {
		navigate_filesystemW();
	}
	else if (arg.size() == 4 && arg[1] == "echoclient") {
		run_echoclient(arg[2], arg[3]);
	}
	else if (arg.size() == 4 && arg[1] == "echoserver") {
		run_echoserver(arg[2], arg[3]);
	}
	else {
		printf("Usage 1: %s fs\n", arg[0].c_str());
		printf("Usage 2: %s echoclient <server address> <server port>\n", arg[0].c_str());
		printf("Usage 3: %s echoserver <listen address> <listen port>\n", arg[0].c_str());
	}
	
	return 0;
}