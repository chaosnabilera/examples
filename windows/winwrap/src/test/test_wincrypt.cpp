#include <iostream>
#include <fstream>
#include <memory>

#include "winwrap_wincrypt.h"

#include "dprintf.hpp"

using namespace std;

#define READBUFLEN (1024*1024)

void encrypt_file_aes256(char* path, char* key) {
	shared_ptr<BYTE> fdata(new BYTE[READBUFLEN], default_delete<BYTE[]>());
	do {
	} while (0);
}