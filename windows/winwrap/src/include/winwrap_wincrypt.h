#pragma once

#include <Windows.h>

#include <memory>

const int AES256_KEY_BYTESIZE = 32;
const int AES_BLOCKSIZE = 16;

const int SHA256_BLOCKSIZE = 32;

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

enum AesMode {
	AES_CBC = CRYPT_MODE_CBC,
	AES_ECB = CRYPT_MODE_ECB
};

bool WinCryptGetAESCryptProvider(HCRYPTPROV& hcryptprov);

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