#include <string>
#include <iostream>

#include <Windows.h>

#define DEBUGGER_DLL_PROC_IMPL
#include "debugger/DebuggerAPI.h"

void loadDebuggerDLL(const TCHAR *dllName, TCHAR *cmdLine);
void unloadDebuggerDLL(bool bForce);
static void initAPI(void);

using namespace std;

namespace
{
	HMODULE gDebuggerDll = NULL;
	bool gDebuggerExited = true;
	IDebugger dbg;

	// This is just a helper, so the DLL will be unloaded when the application exits
	// and we don't need to modify the application code, to call unloading at the right time.
	class Unloader
	{
	public:
		Unloader() {}
		~Unloader()
		{
			unloadDebuggerDLL(true);
		}
	};
	Unloader uld;

	static const char *gRegisterName[] =
	{
		"D0"
		, "D1"
		, "D2"
		, "D3"
		, "D4"
		, "D5"
		, "D6"
		, "D7"

		, "A0"
		, "A1"
		, "A2"
		, "A3"
		, "A4"
		, "A5"
		, "A6"
		, "A7"

		, "IP"
		, "CCR"

	};

	static const ClDbgStatusFlag gStatusFlags[] =
	{
		ClDbgStatusFlag("C", (1 << 0))
		, ClDbgStatusFlag("V", (1 << 1))
		, ClDbgStatusFlag("Z", (1 << 2))
		, ClDbgStatusFlag("N", (1 << 3))
		, ClDbgStatusFlag("X", (1 << 4))
	};
}
IDebugger *Debugger = &dbg;

void loadDebuggerDLL(const TCHAR *dllName, TCHAR *cmdLine)
{
	if (gDebuggerDll)
	{
		// If the debugger has exited by the user, we need to unload the DLL before
		// we can restart it.
		if (gDebuggerExited)
			unloadDebuggerDLL(false);
		else
		{
			// The debugger was still running, so we can just restore it.
			Debugger->ShowDebugger(SW_RESTORE);
			return;
		}
	}

	gDebuggerDll = LoadLibrary(dllName);
	if (!gDebuggerDll)
		return;

	initAPI();

#ifdef _UNICODE
	wstring cmd(cmdLine);
#else
	wstring cmd;
	char *p = cmdLine;
	while (*p)
		cmd += *p++;
#endif

	gDebuggerExited = true;
	wchar_t *error;
	if ((error = InitDebugger(getVersion(), &cmd[0], Debugger)) != nullptr)
	{
		MessageBoxW(NULL, error, L"Error initializing GUI Debugger", MB_OK);
		unloadDebuggerDLL(true);
		return;
	}

	Debugger->ShowDebugger(SW_RESTORE);
	gDebuggerExited = false;
}

void unloadDebuggerDLL(bool force)
{
	if (gDebuggerDll)
	{
		if (force)
		{
			Debugger->CloseDebugger();
			gDebuggerExited = true;
		}

		if (!gDebuggerExited)
			return;
		else
		{
			FreeLibrary(gDebuggerDll);
			gDebuggerDll = NULL;
			gDebuggerExited = true;
		}
	}
}

static void DebuggerExited(void)
{
	gDebuggerExited = true;
}

static size_t DbgMemoryRead(size_t address, ClDbgByte *buffer, size_t bufferSize)
{
	ClDbgByte *s = buffer;
	ClDbgByte *e = buffer+ bufferSize;

	uint8_t c = (address & 0xff);
	while (s < e)
	{
		s->value = c++;
		s->safe = true;
		s++;
	}

	return bufferSize;
}

static size_t DbgMemoryWrite(size_t address, ClDbgByte *buffer, size_t bufferSize)
{
	return bufferSize;
}

static void DbgMachineDescription(ClDbgMachine *machine)
{
	machine->cores = 1;
	machine->name = "Commodore Amiga";
	machine->model = "A1000";
}

static void DbgCoreDescription(ClDbgCPUCore *core, uint32_t idx)
{
	core->name = "Motorola 68k";
	core->model = "M68000";
	core->type = 1;	// To be defined.
	core->registers = sizeof(gRegisterName)/sizeof(gRegisterName[0]);
	core->statusFlags = sizeof(gStatusFlags) / sizeof(gStatusFlags[0]);
	core->isVirtual = false;
}

static void DbgRegisterDescription(ClDbgCPURegister *cpuRegister, uint32_t idx)
{
	cpuRegister->name = gRegisterName[idx];

	if (idx < 17)
		cpuRegister->byteWidth = 4;
	else
		cpuRegister->byteWidth = 1;

	if (idx <= 7)
		cpuRegister->type = ClDbgCPURegister::DATA;
	else if (idx <= 14)
		cpuRegister->type = ClDbgCPURegister::DATA_ADDRESS;
	else if (idx == 15)
		cpuRegister->type = (ClDbgCPURegister::RegisterType)(ClDbgCPURegister::DATA_ADDRESS | ClDbgCPURegister::STACK_POINTER);
	else if (idx == 16)
		cpuRegister->type = ClDbgCPURegister::INSTRUCTION_POINTER;
	else if (idx == 17)
		cpuRegister->type = ClDbgCPURegister::STATUS_FLAGS;
}

static void DbgStatusFlagDescription(ClDbgStatusFlag *statusFlag, uint32_t idx)
{
	*statusFlag = gStatusFlags[idx];
}

void initAPI()
{
	DEBUGGER_DLL_PROC_IMPORT(gDebuggerDll, InitDebugger);

	Debugger->DebuggerExited = DebuggerExited;
	Debugger->MemoryRead = DbgMemoryRead;
	Debugger->MemoryWrite = DbgMemoryWrite;
	Debugger->MachineDescription = DbgMachineDescription;
	Debugger->CoreDescription = DbgCoreDescription;
	Debugger->RegisterDescription = DbgRegisterDescription;
	Debugger->StatusFlagDescription = DbgStatusFlagDescription;

	// We use this dummy so that we can savely call the functions without
	// constantly checking the pointers. Dummy always returns 0.
	DEFAULT_FUNCTION(ShowDebugger);
	DEFAULT_FUNCTION(CloseDebugger);
}
