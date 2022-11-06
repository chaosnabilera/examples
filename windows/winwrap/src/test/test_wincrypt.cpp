#include <Windows.h>

#include <iostream>
#include <fstream>
#include <memory>

#include "winwrap_filesystem.h"
#include "winwrap_wincrypt.h"

#include "dprintf.hpp"

#define READBUFLEN (1024*1024)

char aes256_key[] = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x10\x11\x12\x13\x14\x15\x16\x01\x02\x03\x04\x05\x06\x07\x08\x09\x10\x11\x12\x13\x14\x15\x16";
BYTE iv[16] = { 0 };


bool WWWincryptGenerateRandomFile(std::string path, size_t size) {
    bool result = false;
    const size_t bufsize = 1024 * 1024;
    std::shared_ptr<WinFile> ofile(nullptr);
    std::shared_ptr<BYTE> buf(new BYTE[bufsize], std::default_delete<BYTE[]>());
    size_t written = 0;
    
    do {
        if (!WinPath::openFileA(path, "w", &ofile)) {
            dprintf("[WWWincryptGenerateRandomFile] Failed to open file %s for writing", path.c_str());
            break;
        }

        while (written < size) {
            size_t towrite = min(size - written, bufsize);
            if (!WinCryptGenRandom(buf.get(), towrite)) {
                dprintf("[WWWincryptGenerateRandomFile] WinCryptGenRandom %llu bytes failed", towrite);
                break;
            }    
            if (!ofile->writeBytes(buf.get(), towrite)) {
                dprintf("[WWWincryptGenerateRandomFile] ofile->writeBytes %llu bytes failed", towrite);
                break;
            }
            written += towrite;
        }
        
        if (written == size) {
            result = true;
        }
    } while (0);

    return result;
}

void WWWincryptEncryptFile(std::string src, std::string dst) {
    std::shared_ptr<WinFile> ifile(nullptr);
    std::shared_ptr<WinFile> ofile(nullptr);
    long long ifile_content_size = 0;
    long long ifile_read_result = 0;
    std::shared_ptr<BYTE> buffer(nullptr);
    long long buffer_size = 0;
    std::shared_ptr<WinCryptAES256> aes256(nullptr);
    long long ifile_encrypted_size = 0;
    std::shared_ptr<WinCryptSHA256> sha256(nullptr);
    BYTE sha256_hash[32] = { 0 };

    do {
        aes256 = std::shared_ptr<WinCryptAES256>(
            WinCryptAES256::createWinCryptAES(aes256_key, sizeof(aes256_key), iv, AES_CBC)
        );
        if (aes256.get() == nullptr) {
            printf("Failed to create AES256 obj\n");
            break;
        }

        sha256 = std::shared_ptr<WinCryptSHA256>(
            WinCryptSHA256::createWinCryptSHA256()
        );
        if (sha256.get() == nullptr) {
            printf("Failed to create SHA256 obj\n");
            break;
        }
        
        if (!WinPath::isFileA(src)) {
            printf("%s is not a file!\n", src.c_str());
            break;
        }
        if (!WinPath::openFileA(src, "r", &ifile)) {
            printf("failed to open %s for read\n", src.c_str());
            break;
        }
        if (!ifile->fileSize(&ifile_content_size)) {
            printf("failed to get file size of %s\n", src.c_str());
            break;
        }
        if (ifile_content_size > 0xFFFFFFFF) {
            printf("file size too big : %lld. This test supports only 32bit address space\n", ifile_content_size);
            break;
        }
        printf("size of %s: %lld\n", src.c_str(), ifile_content_size);

        // 2*AES_BLOCKSIZE is for aes256 padding
        buffer_size = ifile_content_size + (2*AES_BLOCKSIZE);
        buffer = std::shared_ptr<BYTE>(new BYTE[buffer_size], std::default_delete<BYTE[]>());
        memset(buffer.get(), 0, buffer_size);

        if (!ifile->readBytes(buffer.get(), buffer_size, ifile_content_size, &ifile_read_result)) {
            printf("failed reading from %s\n", src.c_str());
            break;
        }

        if (!sha256->update(buffer.get(), ifile_content_size)) {
            printf("failed sha256->update\n");
            break;
        }
        if (!sha256->digest(sha256_hash, sizeof(sha256_hash))) {
            printf("failed sha256->digest\n");
            break;
        }
        printf("sha256 hash of input data before encryption: ");
        for (int i = 0; i < sizeof(sha256_hash); ++i)
            printf("%02x", sha256_hash[i]);
        printf("\n");

        if ((ifile_encrypted_size = aes256->encrypt(buffer.get(), ifile_content_size, buffer_size, TRUE)) == 0) {
            printf("failed aes encryption\n");
            break;
        }
        if (!WinPath::openFileA(dst, "w", &ofile)) {
            printf("failed to open %s for write\n", src.c_str());
            break;
        }
        if (!(ofile->writeBytes(buffer.get(), ifile_encrypted_size))) {
            printf("failed to write to %s\n", dst.c_str());
            break;
        }
    } while (0);
}

void WWWincryptDecryptFile(std::string src, std::string dst) {
    std::shared_ptr<WinFile> ifile(nullptr);
    std::shared_ptr<WinFile> ofile(nullptr);
    long long ifile_read_result = 0;
    std::shared_ptr<BYTE> ifile_content(nullptr);
    long long ifile_content_size = 0;
    long long ifile_decrypted_size = 0;
    std::shared_ptr<WinCryptAES256> aes256(nullptr);
    std::shared_ptr<WinCryptSHA256> sha256(nullptr);
    BYTE sha256_hash[32] = { 0 };

    do {
        aes256 = std::shared_ptr<WinCryptAES256>(
            WinCryptAES256::createWinCryptAES(aes256_key, sizeof(aes256_key), iv, AES_CBC)
        );
        if (aes256.get() == nullptr) {
            printf("Failed to create AES256 obj\n");
            break;
        }
        sha256 = std::shared_ptr<WinCryptSHA256>(
            WinCryptSHA256::createWinCryptSHA256()
            );
        if (sha256.get() == nullptr) {
            printf("Failed to create SHA256 obj\n");
            break;
        }
        
        if (!WinPath::isFileA(src)) {
            printf("%s is not a file!\n", src.c_str());
            break;
        }
        if (!WinPath::openFileA(src, "r", &ifile)) {
            printf("failed to open %s for read\n", src.c_str());
            break;
        }
        if (!ifile->fileSize(&ifile_content_size)) {
            printf("failed to get file size of %s\n", src.c_str());
            break;
        }
        if (ifile_content_size > 0xFFFFFFFF) {
            printf("file size too big : %lld. This test supports only 32bit address space\n", ifile_content_size);
            break;
        }
        printf("size of %s: %lld\n", src.c_str(), ifile_content_size);

        ifile_content = std::shared_ptr<BYTE>(new BYTE[ifile_content_size], std::default_delete<BYTE[]>());

        if (!ifile->readBytes(ifile_content.get(), ifile_content_size, ifile_content_size, &ifile_read_result)) {
            printf("failed reading from %s\n", src.c_str());
            break;
        }

        if ((ifile_decrypted_size = aes256->decrypt(ifile_content.get(), ifile_content_size, TRUE)) == 0) {
            printf("failed aes decryption\n");
            break;
        }
        printf("decrypted size of %s: %lld\n", src.c_str(), ifile_decrypted_size);

        if (!sha256->update(ifile_content.get(), ifile_decrypted_size)) {
            printf("failed sha256->update\n");
            break;
        }
        if (!sha256->digest(sha256_hash, sizeof(sha256_hash))) {
            printf("failed sha256->digest\n");
            break;
        }
        printf("sha256 hash of input data after decryption: ");
        for (int i = 0; i < sizeof(sha256_hash); ++i)
            printf("%02x", sha256_hash[i]);
        printf("\n");

        if (!WinPath::openFileA(dst, "w", &ofile)) {
            printf("failed to open %s for write\n", src.c_str());
            break;
        }
        if (!(ofile->writeBytes(ifile_content.get(), ifile_decrypted_size))) {
            printf("failed to write to %s\n", dst.c_str());
            break;
        }
    } while (0);
}