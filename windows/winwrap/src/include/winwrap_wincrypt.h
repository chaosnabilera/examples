#pragma once
#ifndef __WINWRAP_WINCRYPT_H__
#define __WINWRAP_WINCRYPT_H__

#include <Windows.h>

#include <memory>

const int AES256_KEY_BYTESIZE = 32;
const int AES_BLOCKSIZE = 16;

const int SHA256_BLOCKSIZE = 32;

enum AesMode {
    AES_CBC = CRYPT_MODE_CBC,
    AES_ECB = CRYPT_MODE_ECB
};

bool WinCryptGetRSAAESCryptProvider(HCRYPTPROV* out_hcryptprov);

// According to MSDN CryptGenRandom remarks thread ID is used for seeding random
// So this is probably thread safe
bool WinCryptGenRandom(BYTE* buf, int buflen);

class WinCryptAES256 {
public:
    ~WinCryptAES256();
    bool setKey(char* keybuf, int keybuflen, BYTE* iv, AesMode mode);
    // note that once we encrypt, we can't decrypt with it and vice versa
    // return non-zero on success
    DWORD encrypt(BYTE* target, DWORD target_len, DWORD target_buflen, BOOL finalize);
    DWORD decrypt(BYTE* target, DWORD target_len, BOOL finalize); 

    static WinCryptAES256* createWinCryptAES();
    static WinCryptAES256* createWinCryptAES(char* keybuf, int keybuflen, BYTE* iv, AesMode mode);
private:
    WinCryptAES256(HCRYPTPROV hcryptprov);
    HCRYPTPROV hCryptProv;
    HCRYPTKEY hKey;
};

class WinCryptSHA256 {
public:
    ~WinCryptSHA256();
    bool update(BYTE* in_data, DWORD in_data_len);
    bool digest(BYTE* out_buf, DWORD out_buf_size);
    
    static WinCryptSHA256* createWinCryptSHA256();
private:
    WinCryptSHA256(HCRYPTPROV hcryptprov, HCRYPTHASH hhash);
    HCRYPTPROV hCryptProv;
    HCRYPTHASH hHash;
};

#endif