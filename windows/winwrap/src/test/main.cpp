#include <iostream>
#include <string>
#include <vector>

#include <Windows.h>
#include <locale.h>

void WWNavigateFileSystem(bool use_ifileoperation);
void WWCopyFileSimple(std::string src, std::string dst);
void run_echoclient_blockio(std::string server_addr, std::string server_port);
void run_echoclient_nonblockio(std::string server_addr, std::string server_port);
void run_echoserver_blockio(std::string server_addr, std::string server_port);
void run_echoserver_nonblockio(std::string server_addr, std::string server_port);
void WWWincryptEncryptFile(std::string src, std::string dst);
void WWWincryptDecryptFile(std::string src, std::string dst);
bool WWWincryptGenerateRandomFile(std::string path, size_t size);
bool WWTestCompression();
void WWTestRegistry();

void print_usage(std::vector<std::string>& arg) {
    printf("Usage 1: %s fs\n", arg[0].c_str());
    printf("Usage 2: %s copy <src filepath> <dst filepath>\n", arg[0].c_str());
    printf("Usage 3: %s echoclient [block/nonblock] <server address> <server port>\n", arg[0].c_str());
    printf("Usage 4: %s echoserver [block/nonblock]  <listen address> <listen port>\n", arg[0].c_str());
    printf("Usage 5: %s encrypt_file <src filepath> <dst filepath>\n", arg[0].c_str());
    printf("Usage 6: %s decrypt_file <src filepath> <dst filepath>\n", arg[0].c_str());
    printf("Usage 7: %s generate_random <target filepath> <filesize>\n", arg[0].c_str());
    printf("Usage 8: %s test_compression\n", arg[0].c_str());
    printf("Usage 9: %s test_registry\n", arg[0].c_str());
}

int main(int argc, char** argv) {
    std::vector<std::string> arg;

    // unicode characters won't be properly displayed if we omit these
    setlocale(LC_ALL, ".65001");    // for multi-byte
    _wsetlocale(LC_ALL, L".65001"); // for unicode

    for (int i = 0; i < argc; ++i)
        arg.push_back(argv[i]);

    if (arg.size() == 2 && arg[1] == "fs") {
        WWNavigateFileSystem(false);
    }
    else if (arg.size() == 2 && arg[1] == "fs_ifo") {
        WWNavigateFileSystem(true);
    }
    else if (arg.size() == 4 && arg[1] == "copy") {
        WWCopyFileSimple(arg[2], arg[3]);
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
        WWWincryptEncryptFile(arg[2], arg[3]);
    }
    else if (arg.size() == 4 && arg[1] == "decrypt_file") {
        WWWincryptDecryptFile(arg[2], arg[3]);
    }
    else if (arg.size() == 4 && arg[1] == "generate_random") {
        WWWincryptGenerateRandomFile(arg[2], std::stoi(arg[3]));
    }
    else if (arg.size() == 2 && arg[1] == "test_compression") {
        WWTestCompression();
    }
    else if (arg.size() == 2 && arg[1] == "test_registry") {
        WWTestRegistry();
    }
    else {
        print_usage(arg);
    }
    
    return 0;
}