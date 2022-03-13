#include <iostream>
#include <string>
#include <vector>

#include <locale.h>

using namespace std;

void navigate_filesystemW();
void copy_file(string src, string dst);
void run_echoclient_blockio(string server_addr, string server_port);
void run_echoclient_nonblockio(string server_addr, string server_port);
void run_echoserver_blockio(string server_addr, string server_port);
void run_echoserver_nonblockio(string server_addr, string server_port);
void encrypt_file(string src, string dst);
void decrypt_file(string src, string dst);

void print_usage(vector<string>& arg) {
	printf("Usage 1: %s fs\n", arg[0].c_str());
	printf("Usage 2: %s copy <src filepath> <dst filepath>\n", arg[0].c_str());
	printf("Usage 3: %s echoclient [block/nonblock] <server address> <server port>\n", arg[0].c_str());
	printf("Usage 4: %s echoserver [block/nonblock]  <listen address> <listen port>\n", arg[0].c_str());
	printf("Usage 5: %s encrypt_file <src filepath> <dst filepath>\n", arg[0].c_str());
	printf("Usage 6: %s decrypt_file <src filepath> <dst filepath>\n", arg[0].c_str());
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
	else if (arg.size() == 4 && arg[1] == "copy") {
		copy_file(arg[2], arg[3]);
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
	else if (arg.size() == 4 && arg[1] == "encrypt_file") {
		encrypt_file(arg[2], arg[3]);
	}
	else if (arg.size() == 4 && arg[1] == "decrypt_file") {
		decrypt_file(arg[2], arg[3]);
	}
	else {
		print_usage(arg);
	}
	
	return 0;
}