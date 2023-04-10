#include <Windows.h>
#include <iostream>
#include <string>

#include "dprintf.hpp"
#include "module.h"

typedef NTSTATUS(WINAPI *PRtlGetVersion)(LPOSVERSIONINFOEXW);

void get_version_using_rtlgetversion(){
    HMODULE hntdll = NULL;
    OSVERSIONINFOEXW v = { 0 };
    CHAR buffer[512] = { 0 };
    std::string os_name;

	do
	{
		hntdll = GetModuleHandleA("ntdll.dll");
		if (hntdll == NULL) {
			printf("Failed to load ntdll\n");
            break;
		}

		PRtlGetVersion pRtlGetVersion = (PRtlGetVersion)GetProcAddress(hntdll, "RtlGetVersion");
		if (pRtlGetVersion == NULL) {
			printf("Couldn't find RtlGetVersion in ntdll\n");
            break;
		}

		v.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
		if (pRtlGetVersion(&v) != 0) { // 0 == STATUS_SUCCESS
			printf("Unable to get OS version with RtlGetVersion\n");
			break;
		}

		printf("Major   : %u\n", v.dwMajorVersion);
		printf("Minor   : %u\n", v.dwMinorVersion);
		printf("Build   : %u\n", v.dwBuildNumber);
		printf("Maint   : %S\n", v.szCSDVersion);
		printf("Platform: %u\n", v.dwPlatformId);
		printf("Type    : %hu\n", v.wProductType);
		printf("SP Major: %hu\n", v.wServicePackMajor);
		printf("SP Minor: %hu\n", v.wServicePackMinor);
		printf("Suite   : %hu\n", v.wSuiteMask);

		if (v.dwMajorVersion == 3)
		{
			os_name = "Windows NT 3.51";
		}
		else if (v.dwMajorVersion == 4)
		{
			if (v.dwMinorVersion == 0 && v.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
			{
				os_name = "Windows 95";
			}
			else if (v.dwMinorVersion == 10)
			{
				os_name = "Windows 98";
			}
			else if (v.dwMinorVersion == 90)
			{
				os_name = "Windows ME";
			}
			else if (v.dwMinorVersion == 0 && v.dwPlatformId == VER_PLATFORM_WIN32_NT)
			{
				os_name = "Windows NT 4.0";
			}
		}
		else if (v.dwMajorVersion == 5)
		{
			if (v.dwMinorVersion == 0)
			{
				os_name = "Windows 2000";
			}
			else if (v.dwMinorVersion == 1)
			{
				os_name = "Windows XP";
			}
			else if (v.dwMinorVersion == 2)
			{
				os_name = "Windows .NET Server";
			}
		}
		else if (v.dwMajorVersion == 6)
		{
			if (v.dwMinorVersion == 0)
			{
				os_name = v.wProductType == VER_NT_WORKSTATION ? "Windows Vista" : "Windows 2008";
			}
			else if (v.dwMinorVersion == 1)
			{
				os_name = v.wProductType == VER_NT_WORKSTATION ? "Windows 7" : "Windows 2008 R2";
			}
			else if (v.dwMinorVersion == 2)
			{
				os_name = v.wProductType == VER_NT_WORKSTATION ? "Windows 8" : "Windows 2012";
			}
			else if (v.dwMinorVersion == 3)
			{
				os_name = v.wProductType == VER_NT_WORKSTATION ? "Windows 8.1" : "Windows 2012 R2";
			}
		}
		else if (v.dwMajorVersion == 10)
		{
			if (v.dwMinorVersion == 0)
			{
				os_name = v.wProductType == VER_NT_WORKSTATION ? "Windows 10" : "Windows 2016+";
			}
		}
        else{
            os_name = "Unknown";
        }

		if (wcslen(v.szCSDVersion) > 0)
		{
			_snprintf(buffer, sizeof(buffer)-1, "%s (%u.%u Build %u, %S).", os_name.c_str(), v.dwMajorVersion, v.dwMinorVersion, v.dwBuildNumber, v.szCSDVersion);
		}
		else
		{
			_snprintf(buffer, sizeof(buffer)-1, "%s (%u.%u Build %u).", os_name.c_str(), v.dwMajorVersion, v.dwMinorVersion, v.dwBuildNumber);
		}

		printf("Version set to: %s\n", buffer);
	} while (0);
}