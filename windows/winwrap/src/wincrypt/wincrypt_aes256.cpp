
#include "winwrap_wincrypt.h"
#include "dprintf.hpp"

typedef struct _Aes256Key
{
    BLOBHEADER header;
    DWORD length;
    BYTE key[AES256_KEY_BYTESIZE];
} Aes256Key;

WinCryptAES256* WinCryptAES256::createWinCryptAES() {
    HCRYPTPROV hcryptprov = NULL;
    if (!WinCryptGetRSAAESCryptProvider(&hcryptprov)) {
        return nullptr;
    }
    return new WinCryptAES256(hcryptprov);
}

WinCryptAES256* WinCryptAES256::createWinCryptAES(char* keybuf, int keybuflen, BYTE* iv, AesMode mode) {
    bool success = false;
    WinCryptAES256* new_instance = nullptr;
    HCRYPTPROV hcryptprov = NULL;
    do {
        if (!WinCryptGetRSAAESCryptProvider(&hcryptprov)) {
            break;
        }
        new_instance = new WinCryptAES256(hcryptprov);
        if (!new_instance->setKey(keybuf, keybuflen, iv, mode)) {
            break;
        }
        success = true;
    } while (0);

    if (!success) {
        delete new_instance; // hcryptprov is also released
        new_instance = nullptr;
    }
    return new_instance;
}

WinCryptAES256::WinCryptAES256(HCRYPTPROV hcryptprov) : 
    hCryptProv(hcryptprov), hKey(NULL) { }

WinCryptAES256::~WinCryptAES256() {
    if (hKey) {
        CryptDestroyKey(hKey);
    }
    if (hCryptProv) {
        CryptReleaseContext(hCryptProv, 0);
    }
}

bool WinCryptAES256::setKey(char* keybuf, int keybuflen, BYTE* iv, AesMode mode) {
    bool success = false;
    Aes256Key key = { 0 };
    DWORD dmode = mode;

    do {
        if (!hCryptProv) {
            dprintf("attempted set_key without successful get_aes_crypt_provider");
            break;
        }
        if (hKey) {
            CryptDestroyKey(hKey);
            hKey = NULL;
        }
        if (keybuflen < AES256_KEY_BYTESIZE) {
            dprintf("keybuflen: %d < AES256_KEY_BYTESIZE : %d", keybuflen, AES256_KEY_BYTESIZE);
            break;
        }

        key.header.bType = PLAINTEXTKEYBLOB;
        key.header.bVersion = CUR_BLOB_VERSION;
        key.header.aiKeyAlg = CALG_AES_256;
        key.length = AES256_KEY_BYTESIZE;
        memcpy(key.key, keybuf, AES256_KEY_BYTESIZE);

        if (!CryptImportKey(hCryptProv, (BYTE*)&key, sizeof(Aes256Key), 0, 0, &hKey)) {
            dprintf("CryptImportKey failed : %d", GetLastError());
            break;
        }
        if (!CryptSetKeyParam(hKey, KP_MODE, (BYTE*)&dmode, 0)) {
            dprintf("Failed to set mode to %u: %d", dmode, GetLastError());
            break;
        }
        if (!CryptSetKeyParam(hKey, KP_IV, iv, 0)) {
            dprintf("Failed to set IV: %d", GetLastError());
            break;
        }
        success = true;
    } while (0);

    return success;
}

DWORD WinCryptAES256::encrypt(BYTE* target, DWORD target_len, DWORD target_buflen, BOOL finalize) {
    bool success = false;
    DWORD required_len = 0;
    DWORD encrypt_result = 0;
    do {
        if (!hCryptProv || !hKey) {
            dprintf("hcrptprv or hkey is not set");
            break;
        }
        required_len = target_len;
        if (!CryptEncrypt(hKey, 0, finalize, 0, nullptr, &required_len, 0)) {
            dprintf("Failed to determine size of resulting aes crypto");
            break;
        }
        if (target_buflen < required_len) {
            dprintf("target_buflen:%u < required_len:%u", target_buflen, required_len);
            break;
        }
        encrypt_result = target_len;
        if (!CryptEncrypt(hKey, 0, finalize, 0, target, &encrypt_result, target_buflen)) {
            dprintf("CryptEncrypt failed: %d", GetLastError());
            break;
        }
        success = true;
    } while (0);

    if (success)
        return encrypt_result;
    else
        return 0;
}

DWORD WinCryptAES256::decrypt(BYTE* target, DWORD target_len, BOOL finalize) {
    bool success = false;
    DWORD required_len = 0;
    DWORD decrypt_result = 0;
    do {
        if (!hCryptProv || !hKey) {
            dprintf("hcrptprv or hkey is not set");
            break;
        }
        if (target_len % AES_BLOCKSIZE != 0) {
            dprintf("target_len : %d must be a multiple of AES_BLOCKSIZE!", target_len);
            break;
        }
        decrypt_result = target_len;
        if (!CryptDecrypt(hKey, 0, finalize, 0, target, &decrypt_result)) {
            dprintf("CryptDecrypt failed: 0x%08x", GetLastError());
            break;

        }
        success = true;
    } while (0);

    if (success)
        return decrypt_result;
    else
        return 0;
}