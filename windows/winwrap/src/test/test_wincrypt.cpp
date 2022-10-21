#include <iostream>
#include <fstream>
#include <memory>

#include "winwrap_filesystem.h"
#include "winwrap_wincrypt.h"

#include "dprintf.hpp"

using namespace std;

#define READBUFLEN (1024*1024)

char aes256_key[] = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x10\x11\x12\x13\x14\x15\x16\x01\x02\x03\x04\x05\x06\x07\x08\x09\x10\x11\x12\x13\x14\x15\x16";
BYTE iv[16] = { 0 };

void encrypt_file(string src, string dst) {
	WinFile* ifile = nullptr;
	WinFile* ofile = nullptr;
	long long ifile_content_size = 0;
	long long ifile_read_result = 0;
	BYTE* ifile_content = nullptr;
	WinCryptAES aes(aes256_key, AES256_KEY_BYTESIZE, iv, AES_CBC);
	DWORD ifile_encrypted_size = 0;

	do {
		if (!WinPath::isFileA(src)) {
			printf("%s is not a file!\n", src.c_str());
			break;
		}
		if ((ifile = WinPath::openFileA(src, "r")) == nullptr) {
			printf("failed to open %s for read\n", src.c_str());
			break;
		}
		if (!ifile->fileSize(ifile_content_size)) {
			printf("failed to get file size of %s\n", src.c_str());
			break;
		}
		if (ifile_content_size > 0xFFFFFFFF) {
			printf("file size too big : %lld. This test supports only 32bit address space\n", ifile_content_size);
			break;
		}
		printf("size of %s: %lld\n", src.c_str(), ifile_content_size);

		if ((ifile_content = (BYTE*)malloc(ifile_content_size + 32)) == NULL) {
			printf("failed to allocate memory of size %lld\n", ifile_content_size + 32);
			break;
		}
		if (!ifile->readBytes(ifile_content, ifile_content_size + 32, ifile_content_size, ifile_read_result)) {
			printf("failed reading from %s\n", src.c_str());
			break;
		}

		if ((ifile_encrypted_size = aes.encrypt(ifile_content, ifile_content_size, ifile_content_size + 32, TRUE)) == 0) {
			printf("failed aes encryption\n");
			break;
		}
		printf("encrypted size of %s: %u\n", src.c_str(), ifile_encrypted_size);

		if ((ofile = WinPath::openFileA(dst, "w")) == nullptr) {
			printf("failed to open %s for write\n", src.c_str());
			break;
		}
		if (!(ofile->writeBytes(ifile_content, ifile_encrypted_size))) {
			printf("failed to write to %s\n", dst.c_str());
			break;
		}
	} while (0);

	if (ifile_content) {
		free(ifile_content);
	}
}

void decrypt_file(string src, string dst) {
	WinFile* ifile = nullptr;
	WinFile* ofile = nullptr;
	long long ifile_content_size = 0;
	long long ifile_read_result = 0;
	BYTE* ifile_content = nullptr;
	WinCryptAES aes(aes256_key, AES256_KEY_BYTESIZE, iv, AES_CBC);
	DWORD ifile_decrypted_size = 0;

	do {
		if (!WinPath::isFileA(src)) {
			printf("%s is not a file!\n", src.c_str());
			break;
		}
		if ((ifile = WinPath::openFileA(src, "r")) == nullptr) {
			printf("failed to open %s for read\n", src.c_str());
			break;
		}
		if (!ifile->fileSize(ifile_content_size)) {
			printf("failed to get file size of %s\n", src.c_str());
			break;
		}
		if (ifile_content_size > 0xFFFFFFFF) {
			printf("file size too big : %lld. This test supports only 32bit address space\n", ifile_content_size);
			break;
		}
		printf("size of %s: %lld\n", src.c_str(), ifile_content_size);

		if ((ifile_content = (BYTE*)malloc(ifile_content_size)) == NULL) {
			printf("failed to allocate memory of size %lld\n", ifile_content_size);
			break;
		}
		if (!ifile->readBytes(ifile_content, ifile_content_size, ifile_content_size, ifile_read_result)) {
			printf("failed reading from %s\n", src.c_str());
			break;
		}

		if ((ifile_decrypted_size = aes.decrypt(ifile_content, ifile_content_size, TRUE)) == 0) {
			printf("failed aes decryption\n");
			break;
		}
		printf("decrypted size of %s: %u\n", src.c_str(), ifile_decrypted_size);

		if ((ofile = WinPath::openFileA(dst, "w")) == nullptr) {
			printf("failed to open %s for write\n", src.c_str());
			break;
		}
		if (!(ofile->writeBytes(ifile_content, ifile_decrypted_size))) {
			printf("failed to write to %s\n", dst.c_str());
			break;
		}
	} while (0);

	if (ifile_content) {
		free(ifile_content);
	}
}