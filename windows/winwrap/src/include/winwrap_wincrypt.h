#pragma once

#include <Windows.h>

const int AES256_KEY_BYTESIZE = 32;
const int AES_BLOCKSIZE = 16;

enum AesMode {
	AES_CBC = CRYPT_MODE_CBC,
	AES_ECB = CRYPT_MODE_ECB
};

class WinCryptAES {
public:
	WinCryptAES();
	WinCryptAES(char* keybuf, int keybuflen, BYTE* iv, AesMode mode);
	~WinCryptAES();

	bool set_key(char* keybuf, int keybuflen, BYTE* iv, AesMode mode);

	// note that once we encrypt, we can't decrypt with it and vice versa
	// return non-zero on success
	DWORD encrypt(BYTE* target, DWORD target_len, DWORD target_buflen, BOOL finalize); 
	DWORD decrypt(BYTE* target, DWORD target_len, BOOL finalize); 
private:
	HCRYPTPROV hcrptprv;
	HCRYPTKEY hkey;
	bool get_aes_crypt_provider();
};