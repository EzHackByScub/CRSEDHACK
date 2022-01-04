#include "AsmInject.h"
// best site https://defuse.ca/online-x86-assembler.htm

void  AsmInject::CreateAbsJmp(void* adrInMemory, void* addrTo, int stolen)
{
	char absJmp[] = { 0xff, 0x25, 0x00, 0x00, 0x00, 0x00, 0, 0, 0,0, //
	 0, 0, 0, 0 };
	memcpy(&absJmp[6], &addrTo, 8);
	DWORD op;
	VirtualProtect(adrInMemory, sizeof(absJmp) + stolen, PAGE_EXECUTE_READWRITE, &op);
	memcpy(adrInMemory, absJmp, sizeof(absJmp));
	VirtualProtect(adrInMemory, sizeof(absJmp) + stolen, op, &op);
}

__int8* AsmInject::Alloc(ULONG64 memSize)
{
	if (memSize)
		return (__int8*)VirtualAlloc(0, memSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	return nullptr;
}

void AsmInject::WriteShell(AsmInfo* Info, void* ShellAdr, int shellSize)
{
	DWORD op;
	__int8* Buffer = (__int8*)Info->BufferAddress;
	VirtualProtect(Buffer, shellSize, PAGE_EXECUTE_READWRITE, &op); 
	void* BufferEnd = &Buffer[Info->CodeEnd];
	memcpy(BufferEnd, ShellAdr, shellSize); //
	VirtualProtect(Buffer, shellSize, op, &op);
	Info->CodeEnd += shellSize;
}
void AsmInject::Setup(AsmInfo* Info)
{
	__int8* Buffer = Info->BufferAddress;
	if (Info->SaveOrig)
	{
		memcpy(&Buffer[Info->CodeEnd], Info->Address, (14 + Info->Stolen)); //Save Original Bytes  
		Info->CodeEnd += (14 + Info->Stolen);
	}
	void* jmpBackAddr = (void*)((__int64)Info->Address + (14 + Info->Stolen)); // Skip  already executed  bytes 
	AsmInject::CreateAbsJmp(&Buffer[Info->CodeEnd], jmpBackAddr, Info->Stolen); // jmp  jmpBackAddr;
	AsmInject::CreateAbsJmp(Info->Address, Buffer, Info->Stolen); // jmp To Buffer;
}

void  AsmInject::GetRegister(AsmInfo* Info, __int64* movValue, int Register) // (AsmInfo Info, int Register);
{
	DWORD op;
	__int8* Buffer = (__int8*)Info->BufferAddress;
	void* EndMem = &Buffer[Info->CodeEnd];
	__int8 absMov[15] = { 0x53, // push rbx
	0x48, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  movabs rbx,0
	0x48, 0x89, 0x03, // mov [rbx],rax
	0x5B }; // pop rbx  

	if (Register == REGISTER::RBX)
	{
		absMov[0] = 0x50;  	absMov[14] = 0x58;  absMov[2] = 0xB8;  absMov[13] = 0xC3; // 48 89 c3 
	}
	if (Register == REGISTER::RCX)
	{
		absMov[13] = 0x0b;
	};
	if (Register == REGISTER::RDX)
	{
		absMov[13] = 0x13;
	};
	if (Register == REGISTER::RSI)
	{
		absMov[13] = 0x33;
	};
	if (Register == REGISTER::RDI)
	{
		absMov[13] = 0x3b;

	};
	if (Register == REGISTER::RBP)
	{
		absMov[13] = 0x2b;
	};
	if (Register == REGISTER::R8)
	{
		absMov[11] = 0x4c;
		absMov[13] = 0x03; //  4c 89 03  

	}
	if (Register == REGISTER::R9)
	{
		absMov[11] = 0x4c;
		absMov[13] = 0x0b;
	};
	if (Register == REGISTER::R10)
	{
		absMov[11] = 0x4c;
		absMov[13] = 0x13;
	};
	if (Register == REGISTER::R11)
	{
		absMov[11] = 0x4c;
		absMov[13] = 0x1b;
	};
	if (Register == REGISTER::R12)
	{
		absMov[11] = 0x4c;
		absMov[13] = 0x23;
	};
	if (Register == REGISTER::R13)
	{
		absMov[11] = 0x4c;
		absMov[13] = 0x2b; //  4c 89 03  
	};
	if (Register == REGISTER::R14)
	{
		absMov[11] = 0x4c;
		absMov[13] = 0x33; //  4c 89 03  
	};
	if (Register == REGISTER::R15)
	{
		absMov[11] = 0x4c;
		absMov[13] = 0x3b; //  4c 89 03  
	}

	VirtualProtect(EndMem, sizeof(absMov), PAGE_EXECUTE_READWRITE, &op);
	memcpy(EndMem, &absMov, sizeof(absMov));
	__int64 temp = (__int64)EndMem;
	temp += 3;
	memcpy((void*)temp, &movValue, 8);
	VirtualProtect(EndMem, sizeof(absMov), op, &op);
	Info->CodeEnd += sizeof(absMov);
}

// cmpAbsolute Dosent work  
void  AsmInject::cmpAbsolute(AsmInfo* Info, __int64* movValue, int Register)
{
	__int8* Buffer = (__int8*)Info->BufferAddress;
	void* adrInMemory = &Buffer[Info->CodeEnd];
	DWORD op;
	__int8 absCmp[15] = { 0x56, // push rsi
		0x48, 0xbe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rsi,Abs
		0x48, 0x39, 0xC3,
		0x5e  // pop rsi
	};
	if (Register == REGISTER::RAX)
	{
		absCmp[13] = 0x06;
	}
	if (Register == REGISTER::RBX)

	{
		absCmp[13] = 0x06;
	}
	if (Register == REGISTER::RCX)
	{
		absCmp[13] = 0x0e;
	}
	if (Register == REGISTER::RDX)
	{
		absCmp[13] = 0x16;
	}
	if (Register == REGISTER::RSI)
	{
		absCmp[0] = 0x50;  // push rax
		absCmp[14] = 0x30; // cmp [rax],rsi
		absCmp[2] = 0xb8; // movabs rax
		absCmp[13] = 0x58; // pop rax
	}
	if (Register == REGISTER::RDI)
	{
		absCmp[13] = 0x3e;
	}
	if (Register == REGISTER::RBP)
	{
		absCmp[13] = 0x2E;
	}
	if (Register == REGISTER::R8)
	{
		absCmp[11] = 0x4C; absCmp[13] = 0x06;
	}
	if (Register == REGISTER::R9)
	{
		absCmp[11] = 0x4C; absCmp[13] = 0x0E;
	}
	if (Register == REGISTER::R10)
	{
		absCmp[11] = 0x4C; absCmp[13] = 0x16;
	}
	if (Register == REGISTER::R11)
	{
		absCmp[11] = 0x4C; absCmp[13] = 0x1e;
	}
	if (Register == REGISTER::R12)
	{
		absCmp[11] = 0x4C; absCmp[13] = 0x26;
	}
	if (Register == REGISTER::R13)
	{
		absCmp[11] = 0x4C; absCmp[13] = 0x2E;
	}
	if (Register == REGISTER::R14)
	{
		absCmp[11] = 0x4C; absCmp[13] = 0x36;
	}
	if (Register == REGISTER::R15)
	{
		absCmp[11] = 0x4C; absCmp[13] = 0x3e;
	}
	VirtualProtect(adrInMemory, sizeof(absCmp), PAGE_EXECUTE_READWRITE, &op);
	memcpy(adrInMemory, &absCmp, sizeof(absCmp));
	__int64 temp = (__int64)adrInMemory;
	temp += 3;
	memcpy((void*)temp, &movValue, 8);
	VirtualProtect(adrInMemory, sizeof(absCmp), op, &op);
	Info->CodeEnd += sizeof(absCmp);
	return;
}

void  AsmInject::jeShort(AsmInfo* Info, __int8 distance)
{
	__int8* Buffer = (__int8*)Info->BufferAddress;
	void* adrInMemory = &Buffer[Info->CodeEnd];
	if (distance >= INT8_MAX || distance <= INT8_MIN)
	{
		return;
	}
	DWORD op;
	__int8 jeShort[] = { 0x74,0x01 };
	jeShort[1] = { distance };
	VirtualProtect(adrInMemory, sizeof(jeShort), PAGE_EXECUTE_READWRITE, &op);
	memcpy(adrInMemory, &jeShort, sizeof(jeShort));
	VirtualProtect(adrInMemory, sizeof(jeShort), op, &op);
	Info->CodeEnd += sizeof(jeShort);
	return;
}

void  AsmInject::jneShort(AsmInfo* Info, __int8 distance)
{
	if (distance >= INT8_MAX || distance <= INT8_MIN)
	{
		return;
	}
	DWORD op;
	__int8* Buffer = (__int8*)Info->BufferAddress;
	void* adrInMemory = &Buffer[Info->CodeEnd];
	__int8 jneShort[] = { 0x75,0x01 };
	jneShort[1] = { distance };
	VirtualProtect(adrInMemory, sizeof(jneShort), PAGE_EXECUTE_READWRITE, &op);
	memcpy(adrInMemory, &jneShort, sizeof(jneShort));
	VirtualProtect(adrInMemory, sizeof(jneShort), op, &op);
	Info->CodeEnd += sizeof(jneShort);
	return;
}




