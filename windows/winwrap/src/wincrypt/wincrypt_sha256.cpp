#include "winwrap_wincrypt.h"

#include "dprintf.hpp"

WinCryptSHA256* WinCryptSHA256::createWinCryptSHA256() {
    HCRYPTPROV hcryptprov = NULL;
    HCRYPTHASH hhash = NULL;
    WinCryptSHA256* new_instance = nullptr;

    do {
        if (!WinCryptGetRSAAESCryptProvider(&hcryptprov)) {
            break;
        }
        if (!CryptCreateHash(hcryptprov, CALG_SHA_256, 0, 0, &hhash)) {
            break;
        }
        new_instance = new WinCryptSHA256(hcryptprov, hhash);
    } while (0);

    if (!new_instance) {
        if(hhash)
            CryptDestroyHash(hhash);
        if(hcryptprov)
            CryptReleaseContext(hcryptprov, 0);
    }

    return new_instance;
}

WinCryptSHA256::WinCryptSHA256(HCRYPTPROV hcryptprov, HCRYPTHASH hhash) :
hCryptProv(hcryptprov), hHash(hhash) { }

WinCryptSHA256::~WinCryptSHA256() {
    if (hHash) {
        CryptDestroyHash(hHash);
    }
    if (hCryptProv) {
        CryptReleaseContext(hCryptProv, 0);
    }
}

bool WinCryptSHA256::update(BYTE* in_data, DWORD in_data_len) {
    bool result = true;
    if (!CryptHashData(hHash, in_data, in_data_len, 0)) {
        dprintf("[WinCryptSHA256::update] CryptHashData failed: 0x%08x", GetLastError());
    }
    return result;
}

bool WinCryptSHA256::digest(BYTE* out_buf, DWORD out_buf_size) {
    bool result = false;
    do {
        if (out_buf_size < SHA256_BLOCKSIZE) {
            dprintf("[WinCryptSHA256::digest] out_buf_size is too small: %u", out_buf_size);
            break;
        }
        if (!CryptGetHashParam(hHash, HP_HASHVAL, out_buf, &out_buf_size, 0)) {
            dprintf("[WinCryptSHA256::digest] CryptGetHashParam failed: 0x%08x", GetLastError());
            break;
        }
        result = true;
    } while (0);
    
    return result;
}