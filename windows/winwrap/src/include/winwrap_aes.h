#pragma once

#include <Windows.h>

class WinAES { // AES256
public:
	WinAES();
	WinAES();
	~WinAES();
private:
	HCRYPTPROV hcrptprv;
	HKEY hkey;
};