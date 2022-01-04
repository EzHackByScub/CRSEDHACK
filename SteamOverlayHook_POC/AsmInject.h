#pragma once
#include <Windows.h>
#include <iostream>
class AsmInfo
{
public:
    void* Address;
    __int8* BufferAddress;
    bool SaveOrig;
    int CodeEnd;
    int Stolen;
};

class AsmInject
{
public:
    static void Setup(AsmInfo* Info);
    static void CreateAbsJmp(void* adrInMemory, void* addrTo, int stolen);
    static void GetRegister(AsmInfo* Info, __int64* movValue, int Register);
    static void cmpAbsolute(AsmInfo* Info, __int64* movValue, int Register);
    static void jeShort(AsmInfo* Info, __int8 distance);
    static void jneShort(AsmInfo* Info, __int8 distance);
    static void WriteShell(AsmInfo* Info, void* ShellAdr, int shellSize);
    static __int8* Alloc(ULONG64 memSize);
};

namespace REGISTER
{
    enum REGS {
        RAX = 0,
        RBX = 1,
        RCX = 2,
        RDX = 3,
        RSI = 4,
        RDI = 5,
        RBP = 6,
//        RSP = 7,
        R8 = 8,
        R9 = 9,
        R10 = 10,
        R11 = 11,
        R12 = 12,
        R13 = 13,
        R14 = 14,
        R15 = 15,
    };
};

