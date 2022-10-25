#include "winwrap_wincrypt.h"

#include "dprintf.hpp"

bool WinCryptGetAESCryptProvider(HCRYPTPROV& hcryptprov) {
	DWORD err = 0;
	bool success = true;
	for (int i = 0; i < _countof(AesProviders); ++i)
	{
		if (CryptAcquireContext(&hcryptprov, NULL, AesProviders[i].provider, AesProviders[i].type, AesProviders[i].flags))
		{
			dprintf("Managed to acquire the crypt context %d!", i);
			success = true;
			break;
		}
		else {
			err = GetLastError();
			dprintf("Failed to acquire the crypt context %d: %d (%x)", i, err, err);
		}
	}
	return success;
}