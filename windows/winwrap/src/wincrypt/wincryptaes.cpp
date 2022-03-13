
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

typedef struct _Aes256Key
{
	BLOBHEADER header;
	DWORD length;
	BYTE key[AES256_KEY_BYTESIZE];
} Aes256Key;

WinCryptAES::WinCryptAES() : hcrptprv(NULL), hkey(NULL) {
	if (!get_aes_crypt_provider()) {
		dprintf("Failed get_aes_crypt_provider!");
	}
}

WinCryptAES::WinCryptAES(char* keybuf, int keybuflen, BYTE* iv, AesMode mode) {
	bool success = false;
	do {
		if (!get_aes_crypt_provider()) {
			dprintf("Failed get_aes_crypt_provider!");
			break;
		}
		if (!set_key(keybuf, keybuflen, iv, mode)) {
			dprintf("Failed set_key!");
			break;
		}
		success = true;
	} while (0);
}

WinCryptAES::~WinCryptAES() {
	if (hkey)
		CryptDestroyKey(hkey);
	if (hcrptprv)
		CryptReleaseContext(hcrptprv, 0);
}

bool WinCryptAES::set_key(char* keybuf, int keybuflen, BYTE* iv, AesMode mode) {
	bool success = false;
	Aes256Key key = { 0 };
	DWORD dmode = mode;

	do {
		if (!hcrptprv) {
			dprintf("attempted set_key without successful get_aes_crypt_provider");
			break;
		}
		if (hkey) {
			CryptDestroyKey(hkey);
			hkey = NULL;
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

		if (!CryptImportKey(hcrptprv, (BYTE*)&key, sizeof(Aes256Key), 0, 0, &hkey)) {
			dprintf("CryptImportKey failed : %d", GetLastError());
			break;
		}
		if (!CryptSetKeyParam(hkey, KP_MODE, (BYTE*)&dmode, 0)) {
			dprintf("Failed to set mode to %u: %d", dmode, GetLastError());
			break;
		}
		if (!CryptSetKeyParam(hkey, KP_IV, iv, 0)) {
			dprintf("Failed to set IV: %d", GetLastError());
			break;
		}
		success = true;
	} while (0);

	return success;
}

DWORD WinCryptAES::encrypt(BYTE* target, DWORD target_len, DWORD target_buflen, BOOL finalize) {
	bool success = false;
	DWORD required_len = 0;
	DWORD encrypt_result = 0;
	do {
		if (!hcrptprv || !hkey) {
			dprintf("hcrptprv or hkey is not set");
			break;
		}
		required_len = target_len;
		if (!CryptEncrypt(hkey, 0, finalize, 0, nullptr, &required_len, 0)) {
			dprintf("Failed to determine size of resulting aes crypto");
			break;
		}
		if (target_buflen < encrypt_result) {
			dprintf("target_buflen:%d < required_len:%d", target_buflen, required_len);
			break;
		}
		encrypt_result = target_len;
		if (!CryptEncrypt(hkey, 0, finalize, 0, target, &encrypt_result, target_buflen)) {
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

DWORD WinCryptAES::decrypt(BYTE* target, DWORD target_len, BOOL finalize) {
	bool success = false;
	DWORD required_len = 0;
	DWORD decrypt_result = 0;
	do {
		if (!hcrptprv || !hkey) {
			dprintf("hcrptprv or hkey is not set");
			break;
		}
		if (target_len % AES_BLOCKSIZE != 0) {
			dprintf("target_len : %d must be a multiple of AES_BLOCKSIZE!", target_len);
			break;
		}
		decrypt_result = target_len;
		if (!CryptDecrypt(hkey, 0, finalize, 0, target, &decrypt_result)) {
			dprintf("CryptDecrypt failed: %d", GetLastError());
			break;

		}
		success = true;
	} while (0);

	if (success)
		return decrypt_result;
	else
		return 0;
}

bool WinCryptAES::get_aes_crypt_provider() {
	DWORD err = 0;
	bool success = true;
	for (int i = 0; i < _countof(AesProviders); ++i)
	{
		if (CryptAcquireContext(&hcrptprv, NULL, AesProviders[i].provider, AesProviders[i].type, AesProviders[i].flags))
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