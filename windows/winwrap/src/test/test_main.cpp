#include <iostream>
#include <string>

#include <locale.h>

#include "winwrap_encoding.h"
#include "winwrap_filesystem.h"

//#include "dprintf.hpp"

using namespace std;

void navigate_filesystemW();
void run_echoclient_blockio(string server_addr, string server_port);
void run_echoclient_nonblockio(string server_addr, string server_port);
void run_echoserver_blockio(string server_addr, string server_port);
void run_echoserver_nonblockio(string server_addr, string server_port);

void print_usage(vector<string>& arg) {
	printf("Usage 1: %s fs\n", arg[0].c_str());
	printf("Usage 2: %s echoclient [block/nonblock] <server address> <server port>\n", arg[0].c_str());
	printf("Usage 3: %s echoserver [block/nonblock]  <listen address> <listen port>\n", arg[0].c_str());
}

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
	else if (arg.size() == 5 && arg[1] == "echoclient") {
		if (arg[2] == "block") {
			run_echoclient_blockio(arg[3], arg[4]);
		}
		else if(arg[2] =="nonblock") {
			run_echoclient_blockio(arg[3], arg[4]);
		}
		else {
			print_usage(arg);
		}
	}
	else if (arg.size() == 5 && arg[1] == "echoserver") {
		if (arg[2] == "block") {
			run_echoserver_blockio(arg[3], arg[4]);
		}
		else if (arg[2] == "nonblock") {
			run_echoserver_nonblockio(arg[3], arg[4]);
		}
		else {
			print_usage(arg);
		}
	}
	else {
		print_usage(arg);
	}
	
	return 0;
}