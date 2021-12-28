#pragma once
#include <Windows.h>
class Utils
{
public:
	static uintptr_t sigscan(const char* module, const char* pattern);
	static uintptr_t GetAbsoluteAddress(uintptr_t address, int offset, int size);
};


