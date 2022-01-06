#pragma once
#include <Windows.h>
#include "Utils.h"
class Addr
{
public:
	static inline uintptr_t pCamera = Utils::GetAbsoluteAddress(
		Utils::sigscan(0, "48 8B 05 ? ? ? ? 0F 57 C0 0F 11 80 ? ? ? ? 0F 11 80"), //Refaddress
		3, 7);
};
