#pragma once
#ifndef __WINWRAP_WINCRYPT_H__
#define __WINWRAP_WINCRYPT_H__

#include <Windows.h>

#include <memory>

bool WinCryptGetRSAAESCryptProvider(HCRYPTPROV* out_hcryptprov);

// According to MSDN CryptGenRandom remarks thread ID is used for seeding random
// So this is probably thread safe
bool WinCryptGenRandom(BYTE* buf, int buflen);

#endif