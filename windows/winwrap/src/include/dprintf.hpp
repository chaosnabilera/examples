#pragma once

#include <iostream>
#include <windows.h>

#ifdef _DEBUG
#define DEBUG_PRINT_STDOUT
#endif

#ifdef DEBUG_PRINT_STDOUT
#define dprintf(...) dprintf_stdout(__VA_ARGS__)
#else
#define dprintf(...) do{}while(0);
#endif

static _inline void dprintf_stdout(const char* format, ...)
{
	va_list args;
	char buffer[4096];
	size_t len;
	_snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "[%04x] ", GetCurrentThreadId());
	len = strlen(buffer);
	va_start(args, format);
	vsnprintf_s(buffer + len, sizeof(buffer) - len, sizeof(buffer) - len - 3, format, args);
	strcat_s(buffer, sizeof(buffer), "\r\n");
	printf(buffer);
	va_end(args);
}