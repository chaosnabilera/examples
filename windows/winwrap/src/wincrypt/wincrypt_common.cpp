#include "winwrap_wincrypt.h"

#include "dprintf.hpp"

typedef struct _CryptProviderParams
{
    const TCHAR* provider;
    const DWORD type;
    const DWORD flags;
} CryptProviderParams;

const CryptProviderParams AesProviders[] =
{
    {MS_ENH_RSA_AES_PROV, PROV_RSA_AES, 0},
    {MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CRYPT_NEWKEYSET},
    {MS_ENH_RSA_AES_PROV_XP, PROV_RSA_AES, 0},
    {MS_ENH_RSA_AES_PROV_XP, PROV_RSA_AES, CRYPT_NEWKEYSET}
};


bool WinCryptGetRSAAESCryptProvider(HCRYPTPROV* out_hcryptprov) {
    DWORD err = 0;
    bool success = true;
    for (int i = 0; i < _countof(AesProviders); ++i)
    {
        if (CryptAcquireContext(out_hcryptprov, NULL, AesProviders[i].provider, AesProviders[i].type, AesProviders[i].flags))
        {
            dprintf("[WinCryptGetRSAAESCryptProvider] Managed to acquire the crypt context %d!", i);
            success = true;
            break;
        }
        else {
            err = GetLastError();
            dprintf("[WinCryptGetRSAAESCryptProvider] Failed to acquire the crypt context %d: %d (%x)", i, err, err);
        }
    }
    return success;
}

bool WinCryptGenRandom(BYTE* buf, int buflen) {
    static bool tried_get_hcryptprov = false;
    static HCRYPTPROV hcryptprov = NULL;
    if (!tried_get_hcryptprov) {
        WinCryptGetRSAAESCryptProvider(&hcryptprov);
        tried_get_hcryptprov = true;
    }
    if (!hcryptprov) {
        dprintf("[WinCryptGenRandom] Failed to get HCRYPTPROV");
        return false;
    }
    if (!CryptGenRandom(hcryptprov, buflen, buf)) {
        dprintf("[WinCryptGenRandom] CryptGenRandom failed : 0x%08x", GetLastError());
        return false;
    }
    return true;
}